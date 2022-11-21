Example - Modem Information
===========================

This example outputs information about the modem (manufacturer, IMEI, etc), configures an NB-IoT connection to
the carrier packet data network, and then outputs the assigned IP address details.

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
cd examples/ModemInfo
pio run
pio run -e m5stack-core2 -t upload
pio device monitor --baud 115200
```

