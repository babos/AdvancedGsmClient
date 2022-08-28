# AdvancedGsmClient
Arduino-style GSM application clients for SIM7020 that take advantage of on-chip applications such as HTTP(S) that already include IPv6 support.

## Overview

Many modern GSM (Global System for Mobile) communications modules have direct onboard support for protocols such as HTTP(S), MQTT(S), and even LwM2M.

Often these in built protocols will support features like IPv6 and TLS that may be difficult to access from the underlying functions, e.g. the SIM7020 does not have easy IPv6 DNS resolution directly, but the HTTP application supports it.

This library is focussed on ensuring primary IPv6 support, and utilising the built in applications available on communication chips.

The following API interfaces are defined, allowing basic code to be switched between the AdvancedGsm direct implementation and stand alone implementation using a Client interface for the stream.

* GsmModem - The modem is primarily used as a network provider, similar to Wifi or Ethernet, so initialisation is about connecting to the Packet Data Protocol (PDP) network (via a configured Access Point Name).
* TcpClient - TCP client based on the the Arduino Client API, with IPv6 support, https://github.com/arduino/ArduinoCore-API
* HttpClient - Based on ArduinoHttpClient interface, https://github.com/arduino-libraries/ArduinoHttpClient (rather than the Arduino-ESP32 HTTPClient, even though the boards I am using are ESP32 based)
* MqttClient [TODO] - Based on the ArudinoMqttClient interface, https://github.com/arduino-libraries/ArduinoMqttClient (alternative PubSubClient, https://github.com/knolleary/pubsubclient/blob/master/src/PubSubClient.h, or MQTT https://github.com/256dpi/arduino-mqtt)
* Lwm2mClient [TODO] - See https://github.com/eclipse/wakaama/, https://github.com/ivankravets/libWakaamaEmb
* UdpClient - Based on core Udp class.

Different ways to compose an application that uses HTTP, whether the modem just supplies TCP sockets (everything else done in software), a secure TLS connection, or manages the entire HTTP client:

* Application -> GsmHttpClient -> modem
* Application -> ArduinoHttpClient -> GsmSecureClient -> modem
* Application -> ArduinoHttpClient -> TlsClient -> GsmTcpClient -> modem


## Supported platforms, boards, and devices

In general PlatformIO, running in VS Code, has been used for development.

Examples should be relatively easily to directly convert into an Arduino IDE `.ino` file.

Boards developed on:

* M5Stack Core 2
* M5Stack Atom [TODO]
* RAKWireless WisBlock [TODO]

Modems supported;

* SIM7020
* SIM7080 [TODO]

## Examples

Examples are written for PlatformIO (in VS Code), generally for the M5Stack Atom, with the AtomDTU for NB-IoT, which has the SIM7020G.

### Basic modem information

`examples/ModemInfo`

Initialise the modem and output model details, and card details.

Then configure the Access Point Name, enable the radio, and begin.

**NOTE:** This loop in this example is now out of date, as begin() handles checking as much status as it can (see the debug output for timings).

The loop then checks the modem status for if it has signal, if it has connected to an operator, and then when it has started the packet data protocol and started to receive IP addresses.

An IPv4 address is assigned on first connection, as is an IPv6 suffix (if enabled), which allows a link-local address (`fed80:` prefix) to be automatically configured.

After initial configuration with IPv6, once the global prefix is received (e.g. from a router advertisement), the modem also reports it's global IPv6 address.

### HTTP client

`examples/HttpClient`

Plain (unencrypted) HTTP connections are working, including verifying IPv6 with `http://v4v6.ipv6-test.com/api/myip.php` (the site replies with what it sees as your client address; also useful for checking carrier NAT).

Currently HTTPS (TLS) is not working.

Manually following the example in the "SIM7020 Series HTTP(S) Application Note v1.04" documentation, the HTTP client can be created, but when you try and connect it gives an error:

```
... packages 1, 2,3 ...
AT+CHTTPCREATEEXT=0,3268,270,"43646a... last package ...2d2d2d0d0a,0,,0,"
+CHTTPCREATEEXT: 0

OK

AT+CHTTPCON=0
ERROR

AT+CHTTPCREATEEXT?
+CHTTPCREATEEXT: 0,1,https://180.97.33.108/
+CHTTPCREATEEXT: 1,0,(null)
+CHTTPCREATEEXT: 2,0,(null)
+CHTTPCREATEEXT: 3,0,(null)
+CHTTPCREATEEXT: 4,0,(null)
OK
```

### MQTT client

`examples/MqttClient`

Test connections to test.mosquitto.org are working for plain (1883), authenticated (1884), and with a Lets Encrypt public certificate (8886), using an M5Stack Atom with the NB-IoT Atom DTU, which has a SIM7020. Connections have been tested with both dual stack (test.mosquitto.org has IPv6) and IPv4, with Telstra (Australia).

The test program starts the modem, and once the packet data connection is ready it connects (using certificates and username as needed), subscribes to "advgsm/t", sends a message to the same topic, and then waits for the response to be echoed, then disconnects.

This component 

**NOTE:** Note all features, e.g. QoS, have been implemented yet.

### LwM2M Client

TBA.

## Sources

The library is heavily based on code from TinyGsm by Volodymyr Shymanskyy, however it is focussed more on the advanced application capabilities now available in many communications chips, rather than just the low level function.

The coding style is also quite different, have separate header files for the interfaces (with the implementation in code files), using more inheritance and a lot less templating.

Some guidelines for writing the code:

* https://docs.arduino.cc/learn/contributions/arduino-library-style-guide
* https://docs.arduino.cc/learn/contributions/arduino-writing-style-guide
* https://isocpp.github.io/CppCoreGuidelines/

## Implementation notes

### M5Stack


// M5 Atom NB-IoT DTU, Serial1
//#define GSM_TX_PIN 22
//#define GSM_RX_PIN 19

// M5 Atom, PORT.A: GPIO26 I2C0_SDA, GPIO32 I2C0_SCL

// M5 StickC, PORT.A: GPIO32 I2C0_SDA, GPIO33 I2C0_SCL

// M5 Stack Core2, PORT.A (Red): GPIO32 I2C0_SDA, GPIO33 I2C0_SCL
// M5 Stack Core2, PORT.B (Black): GPIO26 DAC2, GPIO36 ADC1_CH0

// M5 Stack Core2, PORT.C (Blue), Serial2: GPIO14 TXD2, GPIO13 RXD2
//#define GSM_TX_PIN 14
//#define GSM_RX_PIN 13

// M5 Stack, PORT.A: GPIO21 I2C0_SDA, GPIO22 I2C0_SCL
// M5 Stack Fire, PORT.B: GPIO26 DAC2, GPIO36 ADC1_CH0

// M5 Stack, PORT.C: GPIO16 RXD2, GPIO17 TXD2
