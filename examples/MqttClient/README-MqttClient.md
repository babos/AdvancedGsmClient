Example - MQTT Client
=====================

This example shows using the MQTT client to connect to a test server.

The test uses test.mosquitto.org, which provides a set of test MQTT endpoints. The server is dual
stack, so can be connected via IPv6 (the server address is 2001:41d0:1:925e::1)

You can read about the different endpoints at: https://test.mosquitto.or

You can test different combinations:

* Packet data protocol: IPv6, dual stack, or IPv4
* Destination port: e.g. combinations of encrypted and/or authenticated

The test code connects to the server, subscribes to "cmd/advgsm/demo/rw/#", publishes a test message to "dt/advgsm/demo/rw/txt", waits a short while for any incoming messages, and then disconnects.

Topics follow the [AWS IoT guidelines](https://docs.aws.amazon.com/whitepapers/latest/designing-mqtt-topics-aws-iot-core/designing-mqtt-topics-aws-iot-core.html) and include the topic type (data or command), category (application and context), thing name/device identifier (also used as the username, 'rw'), and context information (data type), e.g. "dt/<application>/<context>/<thing-name>/<dt-type>"  

You can use a separate client, e.g. Mosquitto, to also connect to the test server and
test receiving and sending messages.

Supported hardware
------------------

* M5 Atom DTU NB-IoT
* M5Stack Core2, with NB-IoT SIM 7020 attached

### Setup of the M5Stack Core2

* The NB-IoT unit uses a Grove connector to Port C (UART / Blue) on the Core 2.


Development requirements
------------------------

* VS Code
* Platform IO plugin


Running the example
-------------------

* Open the PlatformIO Core CLI

Build (both Atom and Core2), then deploy the Core2 version (it needs to be attached), then monitor:

```shell
cd examples/HttpClient
pio run
pio run -e m5stack-core2 -t upload
pio device monitor --baud 115200
```

### Using Mosquitto client to test

To listen for published messages:

```shell
mosquitto_sub -h test.mosquitto.org -t "dt/advgsm/demo/rw/#" -u ro -v
```

In another terminal you can test messages to the device:

```shell
mosquitto_pub -h test.mosquitto.org -t "cmd/advgsm/demo/rw/txt" -u wo -P writeonly -m "Command to device"
```

Example output
--------------

Using IPv6 to connect to test.mosquitto.org on port 8885 (with server certificate and authentication).

Logging set to level 5 (DEBUG) and above, but not showing the serial modem commands debugging.

```
### MQTT client example started at 5119
[5120] <INFO> GsmModemCommon: Begin connection to @telstra.iot (IPV6)
[5126] <DEBUG> SIM7020: Attention success
[5792] <DEBUG> SIM7020: Already configured: @telstra.iot (IPV6)
[5903] <DEBUG> SIM7020: Received Signal Strength Indicator: -83 dBm
[6012] <DEBUG> SIM7020: Registered (1): Home
[6139] <DEBUG> SIM7020: Packet data ready
[6268] <INFO> SIM7020: Local IP Addresses: 2001:8004:4880:0:c377:7a4f:8575:a4c7, fe80:0:0:0:c377:7a4f:8575:a4c7
IP address[0] = 2001:8004:4880:0:c377:7a4f:8575:a4c7
IP address[1] = fe80:0:0:0:c377:7a4f:8575:a4c7
IP address[2] = 127.0.0.1
Ready 1 (60)
### Testing MQTT to: test.mosquitto.org (8885)
[6618] <INFO> SIM7020: Set certificate 0 length 1452 with 24 escaped characters
[6701] <DEBUG> SIM7020: Set certificate error 1 result at index 500 (first error is expected, to clear)
[6929] <DEBUG> SIM7020: Certificate set on attempt 2
[6929] <INFO> SIM7020: MQTT 0 disconnect
[6945] <ERROR> SIM7020: MQTT -1 disconnect failed 2
[6945] <INFO> SIM7020: MQTT creating instance test.mosquitto.org, 8885 (TLS yes)
[10740] <DEBUG> SIM7020: MQTT 0 created
[10849] <INFO> SIM7020: MQTT 0 connect client 'testclient' (user 'rw')
Subscribing
Publishing
Disconnecting
[18450] <INFO> SIM7020: MQTT 0 disconnect
Done
```


Notes
-----

### NB-IoT unit connected to Core2

Manufacturer: SIMCOM_Ltd
Model: SIM7020G
Revision: 1910B03SIM7020G
IMEI: 861518040610866
IMSI: 505016210173155
ICCID: 89610185002710267530

### Atom DTU NB-IoT

Manufacturer: SIMCOM_Ltd
Model: SIM7020G
Revision: 1910B06SIM7020G
IMEI: 861518040875915
IMSI: 505016210173154
ICCID: 89610185002710267463

