#
# pretty much always want debugging symbols included
CFLAGS+= -g
# yell out all warnings and whatnot
CFLAGS+= -Wall -Wextra -Wundef
# optimize!
CFLAGS+= -O
# force 32bit
CFLAGS+= -m32
#
CC:=gcc
#
.PHONY: all clean

all: cat0

% : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -f cat0 *.o
