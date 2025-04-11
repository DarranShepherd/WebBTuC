Web Bluetooth Microcontroller experimentation
=================================

To run the experiments locally they need to be served over TLS

* Run `openssl req -newkey rsa:2048 -new -nodes -x509 -days 3650 -keyout key.pem -out cert.pem` to create a self-signed certificate (one time setup)
* Then start an http-server to serve local files `npx http-server -S -C cert.pem`

Experiment 1
------------
https://127.0.0.1:8080/experiment1.html

Simple exploration of the Web Bluetooth API. Shelly UUIDs discovered by inspecting with the LightBlue iOS app because WebBT needs service ids to be specified up front before discovery, etc.

Experiment 2
------------
Connecting to a Shelly device and making RPC calls as per the Shelly documentation at https://kb.shelly.cloud/knowledge-base/kbsa-communicating-with-shelly-devices-via-bluetoo

