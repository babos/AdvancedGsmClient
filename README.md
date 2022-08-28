# AdvancedGsmClient
Arduino-style GSM application clients for SIM7020 that take advantage of on-chip applications such as MQTT(S) and HTTP(S) that already include IPv6 support.

## Overview

Many modern GSM (Global System for Mobile) communications modules have direct onboard support for protocols such as HTTP(S), MQTT(S), and even LwM2M.

Often these in built protocols will support features like IPv6 and TLS that may be difficult to access from the underlying functions, e.g. the SIM7020 does not have easy IPv6 DNS resolution directly, but the HTTP application supports it.

This library is focussed on ensuring primary IPv6 support, and utilising the built in applications available on communication chips.

The following API interfaces are defined, allowing basic code to be switched between the AdvancedGsm direct implementation and stand alone implementation using a Client interface for the stream.

* GsmModem - The modem is primarily used as a network provider, similar to Wifi or Ethernet, so initialisation is about connecting to the Packet Data Protocol (PDP) network (via a configured Access Point Name).
* TcpClient - TCP client based on the the Arduino Client API, with IPv6 support, https://github.com/arduino/ArduinoCore-API
* HttpClient - Based largely on ArduinoHttpClient interface, https://github.com/arduino-libraries/ArduinoHttpClient (rather than the Arduino-ESP32 HTTPClient, even though the boards I am using are ESP32 based)
* MqttClient - Based largely on on the PubSubClient interface, https://github.com/knolleary/pubsubclient/blob/master/src/PubSubClient.h (as well as bits of ArudinoMqttClient, https://github.com/arduino-libraries/ArduinoMqttClient and 256dpi MQTT, https://github.com/256dpi/arduino-mqtt)
* Lwm2mClient [TODO] - See https://github.com/eclipse/wakaama/, https://github.com/ivankravets/libWakaamaEmb
* UdpClient [TODO] - Based on core Udp class.

There are different ways to compose an application that uses HTTP, whether the modem just supplies TCP sockets (everything else done in software), a secure TLS connection, or manages the entire HTTP client:

1. Application -> GsmHttpClient -> modem
2. Application -> ArduinoHttpClient -> GsmSecureClient -> modem
3. Application -> ArduinoHttpClient -> TlsClient -> GsmTcpClient -> modem

This library is focussed on the first pattern, directly leveraging the protocol support in the modem chip.


## Supported platforms, boards, and devices

In general PlatformIO, running in VS Code, has been used for development.

Boards developed on:

* M5Stack Atom
* M5Stack Core 2 [TODO]
* RAKWireless WisBlock [TODO]

Modems supported;

* SIM7020
* SIM7080 [TODO]

## Examples

Examples are written for PlatformIO (in VS Code), generally for the M5Stack Atom, with the AtomDTU for NB-IoT, which has the SIM7020G.

You need the following:

* VS Code
* Platform IO plugin

To run the examples you will need to modify the settings, usually at the top, to change the Access Point Name to the one used by your mobile network provider (the examples have telstra.iot, which is an Australian telco).

To run:

1. Open the root of the project in VS Code
2. Open the Platform IO console (in VS Code)
3. Change directory to the example, e.g. `cd examples/ModemInfo/`
4. Build, e.g. `pio run -e m5stack-atom`
5. Deploy, e.g. `pio run -e m5stack-atom -t upload` and then monitor `pio device monitor --baud 115200`

Examples should be relatively easily to convert into an Arduino IDE `.ino` file, although I have not done that.

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

You need to configure your APN, and then run the sample, which will connect and then output the IP address as seen by the web server.

If it works, you will get something like:

```
### Ready with IPv6 address 2001:8004:4880:0:bbd8:39a6:a0d5:fd14
Testing HTTP to: v4v6.ipv6-test.com, 80
...
HTTP response code: 200
### PAYLOAD:
2001:8004:4880:0:bbd8:39a6:a0d5:fd14
```


#### TLS issues

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

Tests connection to `test.mosquitto.org` for plain (1883), authenticated (1884), and with a Lets Encrypt public certificate (8886), using an M5Stack Atom with the NB-IoT Atom DTU, which has a SIM7020. Connections have been tested with both dual stack (test.mosquitto.org has IPv6) and IPv4, with Telstra (Australia).

If you change the port to one of the commented out alternatives, then the code will configure certificates and/or usernames as needed (based on the port).

The test program starts the modem, and once the packet data connection is ready it connects (using certificates and username as needed), subscribes to "advgsm/t", sends a message to the same topic, and then waits for the response to be echoed, then disconnects.

By default it will only send one message, and then wait. If everything works, you should see it print out something like:

```
### Testing MQTT to: test.mosquitto.org (1883)
Subscribing
Publishing
Received [advgsm/t]: TestMessage
Disconnecting
Done
```

You can test the PDP type (dual stack vs IPv4), and the different ports on `test.mosquitto.org`, as well as turn the various debugging options on or off.

**NOTE:** Note all features, e.g. QoS, have been implemented yet.

### LwM2M Client

TBA.

## Troubleshooting

### Enabling debug output

Logging is managed in `GsmLog.h`, controlled by the compile flag `ADVGSM_LOG_SEVERITY`.

To configure the logging output destination, you need to set `AdvancedGsmLog.Log` to the print destination (e.g. a serial connection) as part of `setup()` (anything that supports `Print` will work):

```c++
void setup() {
  #if ADVGSM_LOG_SEVERITY > 0
  AdvancedGsmLog.Log = Serial;
  #endif
  ...
```

This property only exists if `ADVGSM_LOG_SEVERITY` is set greater than zero, hence the compile guard.

The default settings is 9, which will output all Info logs and above (it does not output Debug or Trace logs). Severity levels are based on the [OpenTelemetry](https://opentelemetry.io/docs/reference/specification/logs/data-model/#field-severitynumber) common levels.

To change the logging level, you need to set the compile flag in the `platformio.ini` file, e.g. to turn on Debug (level 5) and above:

```ini
build_flags =
  '-D ADVGSM_LOG_SEVERITY=5'
```

Note: You can't set this #define in your source code, as the Advanced GSM code will be compiled long before your files are loaded; you need to set it in the build flags (or in the Advanced GSM source code).

If you set `ADVGSM_LOG_SEVERITY=0` then logging will be turned off (and not even compiled into the code, if you really need to save space). Leaving `AdvancedGsmLog.Log` unset will also turn off logging, but they will still exist as empty operations (unless the compiler optimises them).

Logging in the libary source code is through the `ADVGSM_LOG()` macro, which has takes the severity level, a category/tag name, and then a message and optional arguments with `printf` formatting:

```c++
  ADVGSM_LOG(GsmSeverity::Info, "GsmModemCommon",
             GF("Begin connection to %s@%s (%d)"), this->user_name,
             this->access_point_name, this->pdp_type);
```

This is output to the monitoring serial channel (or other print destination) with the milliseconds timestamp, level, category, and then the formatted message:

```
[5329] <INFO> GsmModemCommon: Begin connection to @telstra.iot (2)
```

### Enabling AT command tracing

You can use the `StreamDebugger` from the TinyGsm libraries to dump all the AT commands for additional troubleshooting.

To configure this, add the dependency to your `platformio.ini` file:

```ini
lib_deps =
  vshymanskyy/StreamDebugger
```

Then you can use something like the following (with a compile flag to easily enable/disable), where `Serial` is your monitoring output, and `Serial1` is the connection to the modem where the AT commands will be sent. 

When you configure the modem with the debugger (instead of directly to `Serial1`) it echoes everything both to and from `Serial1` to the monitoring `Serial`.

```c++
#define DUMP_AT_COMMANDS

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(Serial1, Serial);
TestModem testModem(debugger);
#else
TestModem testModem(Serial1);
#endif
```


## Source acknowledgements

The library is heavily based on code from TinyGsm by Volodymyr Shymanskyy, however it is focussed more on the advanced application capabilities now available in many communications chips, rather than just the low level function.

The coding style is also quite different, have separate header files for the interfaces (with the implementation in code files), using more inheritance and a lot less templating.

Some guidelines for writing the code:

* https://docs.arduino.cc/learn/contributions/arduino-library-style-guide
* https://docs.arduino.cc/learn/contributions/arduino-writing-style-guide
* https://isocpp.github.io/CppCoreGuidelines/

The interfaces for each client library are based on those commonly used, e.g. the TCP interface is based on the Arduino core Client API (and can be used as a Client), and the implementations in various Ethernet and WiFi networking stacks.

HTTP is based on ArduinoHttpClient, with influence from Arduino-ESP32 HTTPClient.

The MQTT interface is based on PubSubClient, with some features from ArduinoMqttClient and the 256dpi arduino-mqtt library.


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
