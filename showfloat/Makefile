#
#
OBJS=showfloat
#
CFLAGS+= -Wall -Wextra -Wundef
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
