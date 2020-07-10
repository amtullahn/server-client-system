#Usage:  make -f example04.make
# Makes:  proj04
#

proj12.client: proj12.client.o
	g++ proj12.client.o -o proj12.client

proj12.client.o: proj12.client.c
	g++ -Wall -c proj12.client.c

clean:
	rm -f proj12.client.o
	rm -f proj12


