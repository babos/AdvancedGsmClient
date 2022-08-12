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

## Sources

The library is heavily based on code from TinyGsm by Volodymyr Shymanskyy, however it is focussed more on the advanced application capabilities now available in many communications chips, rather than just the low level function.

The coding style is also quite different, have separate header files for the interfaces (with the implementation in code files), using more inheritance and a lot less templating.

Some guidelines for writing the code:

* https://docs.arduino.cc/learn/contributions/arduino-library-style-guide
* https://docs.arduino.cc/learn/contributions/arduino-writing-style-guide
* https://isocpp.github.io/CppCoreGuidelines/
