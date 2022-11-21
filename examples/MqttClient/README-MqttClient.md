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

