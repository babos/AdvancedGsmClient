Example - HTTP Client
=====================

This example shows using the HTTP client to connect to a web server and retrieve content.

The test uses the ipv6-test.com API, which returns the remote IP address seen by the server.

You can test different combinations:

* Packet data protocol: IPv6, dual stack, or IPv4
* Destination server: v6, dual stack, or v4

Note that using the Telstra mobile network you can make an IPv6 only connection, and still
connect to an IPv4 only server (v4.ipv6-test.com), because the network implements DNS64 + NAT64.

DNS lookup for v4.ipv6-test.com will return the IPv6 DNS64 address 64:ff9b::334b:4e67, and when
the IPv6 only host connects to that address, the Telstra network will use NAT64 to connect to
the address.

Note that the returned address seen by the API call is an IPv4 address from the Telstra carrier grade
NAT, e.g. 1.125.29.153.

Telstra does not provide public IPv4 addresses, so if you connect via IPv4 you also have to go
through NAT (although NAT44 instead of NAT64) to connect to an IPv4 server.

Connections to the IPv6 server are direct, i.e. the API returns the IPv6 address of the device.

NOTE: Currently HTTPS (TLS) is not working.

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

