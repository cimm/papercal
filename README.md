# PaperCal

PaperCal is a calendar application for [Paperd.Ink](https://paperd.ink/) Classic devices. It connects to a [CalDAV](https://en.wikipedia.org/wiki/CalDAV) calendar and displays the upcoming events.

PaperCal is under active development, it kind of works for me but is not production ready software.

PaperCal only works on the Paperd.Ink Classic. The Merlot is not supported (I don’t own one). Open an [issue](https://github.com/cimm/papercal/issues) if you would like to see support for the Paperd.Ink Merlot.

## Installation

You can use the Arduino IDE to compile and upload the sketch, but I prefer the [arduino-cli](https://arduino.github.io/arduino-cli/0.29/). First clone this git repository:

```sh
$ git clone https://github.com/cimm/papercal.git --depth 1
```

Don’t forget to add you WiFi and CalDAV credentials to the sketch’ `papercal/config.hpp` file.

Next install and configure arduino-cli for the ESP32 board, add the required Arduino libraries:

```sh
$ arduino-cli config init
$ arduino-cli config set board_manager.additional_urls https://dl.espressif.com/dl/package_esp32_index.json
$ arduino-cli core update-index
$ arduino-cli core install esp32:esp32
$ arduino-cli lib install GxEPD2
```

Finaly, compile and upload the sketch:

```sh
$ arduino-cli compile --fqbn esp32:esp32:esp32 papercal/
$ arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 papercal/
```

The board will reboot, give it a few seconds, and if everything works you shoud see your upcoming events.

Use the Arduino Serial Monitor extra debug info:

```sh
$ arduino-cli monitor -p /dev/ttyUSB0
```

## Acknowledgement

A good chunk of the code is inspired on the official [Paperd.Ink Library for Arduino](https://github.com/paperdink/PaperdInk-Library). Besides the libraries above, it also includes a copy of the [TinyXML2](https://github.com/leethomason/tinyxml2) parser.
