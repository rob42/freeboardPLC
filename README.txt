
Commandline to load arduino on USB0

/arduinoWorkspace/FreeBoardPLC/Release$ /home/robert/boat/freeboard/arduino-1.0/hardware/tools/avrdude -patmega1280 -carduino -P/dev/ttyUSB0 -b57600 -D -v -v -v -v -Uflash:w:FreeBoardPLC.hex:a -C/home/robert/boat/freeboard/arduino-1.0/hardware/tools/avrdude.conf
