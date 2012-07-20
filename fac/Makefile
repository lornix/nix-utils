#
#
OBJS=fac
#
CFLAGS+= -Wall -Wextra -Wunused
CFLAGS+= -g
#
LDFLAGS+= -lgmp
#
CC:=gcc
#
.PHONY: all clean

all: $(OBJS)

% : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -f $(OBJS)
