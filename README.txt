
Commandline to load arduino on USB0

/arduinoWorkspace/FreeBoardPLC/Release$ /home/robert/boat/freeboard/arduino-1.0/hardware/tools/avrdude -patmega1280 -carduino -P/dev/ttyUSB0 -b57600 -D -v -v -v -v -Uflash:w:FreeBoardPLC.hex:a -C/home/robert/boat/freeboard/arduino-1.0/hardware/tools/avrdude.conf







WARNING:  RXTX Version mismatch
	Jar version = RXTX-2.2pre1
	native lib Version = RXTX-2.2pre2
windKnob - GKnob event occured 5212737: value=51
stallKnob - GKnob event occured 5212737; value=45
windFactorKnob - GKnob event occured 5212739; value=2200000
java.net.UnknownHostException: processing.org
	at java.net.PlainSocketImpl.connect(PlainSocketImpl.java:195)
	at java.net.SocksSocketImpl.connect(SocksSocketImpl.java:366)
	at java.net.Socket.connect(Socket.java:529)
	at java.net.Socket.connect(Socket.java:478)
	at sun.net.NetworkClient.doConnect(NetworkClient.java:163)
	at sun.net.www.http.HttpClient.openServer(HttpClient.java:394)
	at sun.net.www.http.HttpClient.openServer(HttpClient.java:529)
	at sun.net.www.http.HttpClient.<init>(HttpClient.java:233)
	at sun.net.www.http.HttpClient.New(HttpClient.java:306)
	at sun.net.www.http.HttpClient.New(HttpClient.java:323)
	at sun.net.www.protocol.http.HttpURLConnection.getNewHttpClient(HttpURLConnection.java:970)
	at sun.net.www.protocol.http.HttpURLConnection.plainConnect(HttpURLConnection.java:911)
	at sun.net.www.protocol.http.HttpURLConnection.connect(HttpURLConnection.java:836)
	at sun.net.www.protocol.http.HttpURLConnection.getInputStream(HttpURLConnection.java:1172)
	at java.net.URL.openStream(URL.java:1010)
	at processing.app.UpdateCheck.readInt(UpdateCheck.java:131)
	at processing.app.UpdateCheck.updateCheck(UpdateCheck.java:93)
	at processing.app.UpdateCheck$1.run(UpdateCheck.java:63)
	at java.lang.Thread.run(Thread.java:662)
Config: len 39, @C