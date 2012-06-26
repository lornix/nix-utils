#
#
OBJS=fac
#
CFLAGS+= -Wall -Wextra -Wundef
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
