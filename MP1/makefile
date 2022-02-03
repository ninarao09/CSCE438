clean:
	rm -f *.o crc crsd

all: client server

server: crsd.c
	gcc -g -w -Wall -O1 -std=c++11 -lpthread -o crsd crsd.c
	
client: crc.c
	gcc -g -w -Wall -O1 -std=c++11 -lpthread -o crc crc.c
