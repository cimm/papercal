# SynoPaperCal

SynoPaperCal is an application for [Paperd.Ink](https://paperd.ink/) Classic devices. It connects to a [Synology Calendar](https://www.synology.com/en-us/dsm/feature/calendar) and displays today’s events. SynoPaperCal is still being developed, it kind of works for me but is not production ready software yet.

## Library Dependencies

You’ll need to install the following libraries via the Arduino [library manager](https://docs.arduino.cc/software/ide-v2/tutorials/ide-v2-installing-a-library) to build the application. Also install the library dependencies when the Arduino IDE asks for it.

- GxEPD2 by Jean-Marc Zingg
- ArduinoJSON by Benoit Blanchon

## Limitations

- Only works on Paperd.Ink Classic devices. The Merlot is not supported since I don’t have one to test on.
- Does not support TLS for now.
- Does not support recurring events for now.