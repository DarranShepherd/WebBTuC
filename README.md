Web Bluetooth Microcontroller experimentation
=================================

To run the experiments locally they need to be served over TLS

* Run `openssl req -newkey rsa:2048 -new -nodes -x509 -days 3650 -keyout key.pem -out cert.pem` to create a self-signed certificate (one time setup)
* Then start an http-server to serve local files `npx http-server -S -C cert.pem`

Experiment 1
------------
[Local](https://127.0.0.1:8080/experiment1.html) | [GitHub](https://darranshepherd.co.uk/WebBTuC/experiment1.html) 

Simple exploration of the Web Bluetooth API. Shelly UUIDs discovered by inspecting with the LightBlue iOS app because WebBT needs service ids to be specified up front before discovery, etc.

Experiment 2
------------
[Local](https://127.0.0.1:8080/experiment2.html) | [GitHub](https://darranshepherd.co.uk/WebBTuC/experiment2.html) 

Connecting to a Shelly device and making RPC calls as per the Shelly documentation at https://kb.shelly.cloud/knowledge-base/kbsa-communicating-with-shelly-devices-via-bluetoo

Experiment 3
------------
[Local](https://127.0.0.1:8080/experiment3.html) | [GitHub](https://darranshepherd.co.uk/WebBTuC/experiment3.html) | [Arduino Sketch](https://github.com/DarranShepherd/WebBTuC/blob/main/experiment3/experiment3.ino)

A simple Arduino sketch that advertises a BLE service with charateristics that support a basic API to scan for networks, then set the selected network and password. The experiment page allows entering these and writing them to the characteristic, then retrieves the IP address from the characteristic, displays status and makes an fetch request to an API served by the sketch.

Experiment 4
------------
A more polished experiment refining Experiment 3 with more nicely factored code and error handling. Furthering the example with an API implementation to read a sensor ([internal temperature](https://forum.arduino.cc/t/esp32-s2-built-in-temperature-sensor/1237935)?) and toggle a pin (LED).