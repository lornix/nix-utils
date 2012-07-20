#
#
OBJS=showerr
#
CFLAGS+= -Wall -Wextra -Wunused
CFLAGS+= -g
CFLAGS+= -m32
#
LDFLAGS+=
#
CC:=gcc
#
.PHONY: all clean

all: $(OBJS)

% : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -f $(OBJS)
