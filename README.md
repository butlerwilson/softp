A high performance ftp server. We use the zero copy functions to transport
the data. And we define our own protocol to keep the data complete.

Our own protocol is written in ./document/protocol file. And all data
structure is define in ./include/db.h file.

client usage:
	$softp username@hostname
	softp-wr-youngcy>download filename
	softp-wr-youngcy>upload filename
	softp-wr-youngcy>exit
