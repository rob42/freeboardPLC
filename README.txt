NOTES:

Current build is done on a Kubuntu 12.10 linux laptop. It should build on any OS with a reasonably current version of eclipse IDE.

If you just want the binary hex files to upload to the arduino mega they are in the Release[Mega_model] directory.

**You can upload these with the installer https://github.com/rob42/freeboard-installer if you dont want to compile code.

You can configure the GPS type, and serial baud rates as follows:

See CONFIG.txt to configure serial baud rates etc if the defaults dont suit.

Develop and Compile 

To  open and compile the project:

Get Eclipse from http://eclipse.org/ and install it anywhere, but usually in C:\eclipse for windows. DONT pick a directory with spaces, and dont use any in projects - it will cause you grief :-)
In eclipse install the AVR (http://www.baeyens.it/eclipse/) and eGit plugins - google will help. The 'other' avr plugin (http://avr-eclipse.sourceforge.net) will give all sorts of problems as the project expects the baeyens version.

Follow the instructions from github to install git on your PC, and clone this project and your chosen Arduino_ATmegaxxxx project to you local repository.

Start eclipse and File>Import>From Git and follow the wizard. You should end up with two projects in your workspace.
On the  Arduino_ATmegaxxxx project Right-click and Clean, then Build Project.

It should complete without errors, if not look in the Console tab for the full build log. There are some hints in COmpileError.txt.
You can email me if needed. 

Do the same for the FreeboardPLC project. 
There are two options: right-click project>Build configurations>Set Active>(choose your board)

I found eclipse would not upload for me, so I use the commandline to load arduino on USB0. Install the arduino environment, and upload any simple example sketch. In the console window you will see the commandline used. Copy and adjust.

For me, on linux:

FREEBOARD_HOME= ~/gitrep/freeboardPLC
ARDUINO_HOME=/home/robert/dev/arduino-1.0.2/

cd $FREEBOARD_HOME/Release1280
~/gitrep/freeboardPLC/Release1280$ $ARDUINO_HOME/hardware/tools/avrdude -patmega1280 -carduino -P/dev/ttyUSB0 -b57600 -D -v -v -v -v -Uflash:w:FreeBoardPLC.hex:a -C$ARDUINO_HOME/hardware/tools/avrdude.conf




