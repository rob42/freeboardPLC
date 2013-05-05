NOTES:

Current build is done on a Kubuntu 12.04 linux laptop. 

Commandline to load arduino on USB0, as eclipse builds ok, but fails to load it to the arduino

FREEBOARD_HOME= ~/gitrep/freeboardPLC
ARDUINO_HOME=/home/robert/dev/arduino-1.0.2/

cd $FREEBOARD_HOME/Release
~/gitrep/freeboardPLC/Release$ $ARDUINO_HOME/hardware/tools/avrdude -patmega1280 -carduino -P/dev/ttyUSB0 -b57600 -D -v -v -v -v -Uflash:w:FreeBoardPLC.hex:a -C$ARDUINO_HOME/hardware/tools/avrdude.conf

