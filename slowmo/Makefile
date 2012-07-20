#
# Generic compiles-it-all Makefile
#  Loni Nix <lornix@lornix.com>
#    Last modified 2011-08-17
#
# adhere to a higher standard!
#CFLAGS+=-std=c99
# pretty much always want debugging symbols included
CFLAGS+=-g
# yell out all warnings and whatnot
CFLAGS+=-Wall -Wextra -Wunused
# make all warnings into errors
#CFLAGS+=-Werror
# optimize!
#CFLAGS+=-O3
# or not!
CFLAGS+=-O0
#
# das linker flags
# LDFLAGS+=
#
CC:=gcc
#
.SUFFIXES:
.SUFFIXES: .c .o
#
SHELL=/bin/sh
#
CSRC:=$(wildcard *.c)
CPRG:=$(basename $(CSRC))
COBJ:=$(addsuffix .o,$(CPRG))
#
.PHONY: all clean

all: $(CPRG)

% : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -f $(CPRG)
