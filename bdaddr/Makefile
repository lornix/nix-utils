#
# makefile to create bdaddr
#
# didn't find 32bit libbluetooth on 64bit system
# so this one MUST be compiled on 32bit system
# AND linked 'static', to run on 64bit systems

CC=gcc
#CFLAGS=-m32
LDFLAGS=-static -lbluetooth

default: bdaddr

bdaddr: bdaddr.o oui.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f bdaddr
	rm -f *.o
