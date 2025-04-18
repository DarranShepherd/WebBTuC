#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>
#include <esp_mac.h>

#define WIFI_SERVICE_UUID                       "f906af59-6e7a-425a-9483-5b3b114e7e1e"

// Reuse SCAN for SCAN_COUNT
#define WIFI_CHARACTERISTIC_UUID_SCAN           "552ed7bb-704e-4b45-a566-628bc1cfdfd3"
#define WIFI_CHARACTERISTIC_UUID_SCAN_TEMPLATE  "a82e4198-1784-47d2-9d67-127ae09f99d%X"
#define WIFI_CHARACTERISTIC_UUID_IPADDR         "7795f5ce-cbf2-4d1e-a51b-b13006dbea2e"

#define WIFI_CHARACTERISTIC_UUID_SSID       "be5bd74b-09ec-4759-9d7b-cde46f1ecbe0"
#define WIFI_CHARACTERISTIC_UUID_PASSWORD   "a376c353-31f5-42ee-846a-d4551d3b9c56"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharScan;
BLECharacteristic *pCharScanSsids[10];
BLECharacteristic *pCharIpAddr;

BLECharacteristic *pCharSsid;
BLECharacteristic *pCharPassword;

bool deviceConnected = false;
bool btWasConnected = false;

bool wifiConnected = false;

char deviceId [30];

String ssid;
String password;

WiFiServer server(80);
// Variable to store the HTTP request
String header;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Client disconnected - readvertising.");
      deviceConnected = false;
      pServer->startAdvertising();
    }
};

class ScanCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    uint8_t *data = pCharacteristic->getData();
    Serial.printf("Scan characteristic value written %d \n", *data);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    int connected = 0;
    int scanCount = WiFi.scanNetworks();
    Serial.print(scanCount);
    Serial.println(" networks found");

    for (int i = 0; i < scanCount; i++) {
      if (i > 9) break;

      String ssid;
      uint8_t encryptionType;
      int32_t RSSI;
      uint8_t* BSSID;
      int32_t channel;
      WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel);
      Serial.printf("%d: %s, Ch:%d (%ddBm) %d\n", i + 1, ssid.c_str(), channel, RSSI, encryptionType);
      pCharScanSsids[i]->setValue(ssid);
    }
    pCharacteristic->setValue(scanCount);
  }
};
class SsidCallback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      ssid = pCharacteristic->getValue();
      Serial.println(ssid);
    }
};
class PasswordCallback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      password = pCharacteristic->getValue();
      Serial.println(password);

      if (!ssid.isEmpty() && !password.isEmpty()) {
        WiFi.begin(ssid, password);
      }
    }
};

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  uint8_t mac[8];
  esp_efuse_mac_get_default(mac);
  sprintf(deviceId, "DMS Gadget (%02X%02X%02X%02X%02X%02X%02X%02X)", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]);
  BLEDevice::init(deviceId);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());
  pService = pServer->createService(BLEUUID(WIFI_SERVICE_UUID), 32);
  pCharScan = pService->createCharacteristic(
                                   WIFI_CHARACTERISTIC_UUID_SCAN,
                                   BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
                                 );
  pCharScan->setCallbacks(new ScanCallback());

  char ssidCharacteristicUuids[10][37];
  for (int i = 0; i < 10; i++) {
    sprintf(ssidCharacteristicUuids[i], WIFI_CHARACTERISTIC_UUID_SCAN_TEMPLATE, i);
    pCharScanSsids[i] = pService->createCharacteristic(ssidCharacteristicUuids[i], BLECharacteristic::PROPERTY_READ);
  }

  pCharIpAddr = pService->createCharacteristic(
                                         WIFI_CHARACTERISTIC_UUID_IPADDR,
                                         BLECharacteristic::PROPERTY_READ
                                      );

  pCharSsid = pService->createCharacteristic(WIFI_CHARACTERISTIC_UUID_SSID, BLECharacteristic::PROPERTY_WRITE);
  pCharPassword = pService->createCharacteristic(WIFI_CHARACTERISTIC_UUID_PASSWORD, BLECharacteristic::PROPERTY_WRITE);
  pCharSsid->setCallbacks(new SsidCallback());
  pCharPassword->setCallbacks(new PasswordCallback());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(WIFI_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.printf("Device %s advertising\n", deviceId);
}

uint8_t led = HIGH;
void loop() {
  digitalWrite(LED_BUILTIN, led);
  led = (led == HIGH) ? LOW : HIGH;
  delay(250);
  if (!deviceConnected && btWasConnected) {
    pServer->startAdvertising();
  }

  if (deviceConnected && !btWasConnected) {
    btWasConnected = deviceConnected;
  }

  if (!wifiConnected && WiFi.status() == WL_CONNECTED)
  {
    wifiConnected = true;
    pCharIpAddr->setValue(WiFi.localIP().toString());
    server.begin();
  }

  if (wifiConnected) {
    WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println("Connection: close");
            client.println("Access-Control-Allow-Origin: *");
            client.println("Access-Control-Allow-Private-Network: true");
            client.println("Private-Network-Access-ID: 01:02:03:04:05:06");
            client.printf("Private-Network-Access-Name: %s\n", deviceId);
            client.println();

            client.printf("{ \"deviceId\": \"%s\", \"millis\": %d}\n", deviceId, millis());
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  }
}
