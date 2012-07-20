#
#
OBJS=check_my_ip
#
CFLAGS+= -Wall -Wextra -Wunused
CFLAGS+= -g
#
LDFLAGS+= -lcurl
#
CC:=gcc
#
.PHONY: all clean

all: $(OBJS)

% : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -f $(OBJS)
