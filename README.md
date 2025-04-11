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
Connecting to a Shelly device and making RPC calls as per the Shelly documentation at https://kb.shelly.cloud/knowledge-base/kbsa-communicating-with-shelly-devices-via-bluetoo

Experiment 3
------------
A simple Arduino sketch that advertises, has writeable characteristics for SSID and Password. Experiment page allows entering these and writing them to the characteristic, then subscribes to connected / IP address characteristics and displays status.

Experiment 4
------------
Onboarding as in Experiment 3, sketch also launches an HTTP server with a very simple JSON API. Once connected, the page allows calling the API via XHR. Read sensor ([internal temperature](https://forum.arduino.cc/t/esp32-s2-built-in-temperature-sensor/1237935)?), write pin (LED).