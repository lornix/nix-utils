#
#
OBJS=showfloat
#
CFLAGS+= -Wall -Wextra -Wunused
CFLAGS+= -g
#
LDFLAGS+= -lm
#
CC:=gcc
#
.PHONY: all clean

all: $(OBJS)

% : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) *.o
