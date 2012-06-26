# nix-utils

A collection of various utilities I've built over the years.  Some actually useful more than once!

## bdaddr

bdaddr - Utility for changing the Bluetooth device address
Usage:
    bdaddr [-i <dev>] [-r] [-t] [new bdaddr]

## cat0

Streams input to output, or from given file to output, converting <CR> to
<00>.  Handy for using with *xargs*

## check_my_ip

Since I host my own webpage from home, and I'm on the usual (cheap) dynamic
dhcp based IP addressing, my IP occasionally changes.  To quickly check what
my DNS entry is pointing at, I grab the external IP from my DSL modem
(192.168.0.1) and compare it to the DNS request... with a simple yes/no as a
result.  Also reflected in the exit status of the application.

## fac

Factors of a number.  Handles REALLY big numbers. (factor from BSDUtils was
limited)

## showerr

Show error code text associated with a particular error code value.  Wrote this
because so many programs (and function calls) return "errorcode 17"... which
this turns into: `17: File exists`  So helpful!

## showfloat

This turns a number into the hex representation for single precision, double
precision floating point values, long int and binary.  Handy for tinkering in
programs with debuggers.

## slowmo

Slows down a program by adding delays during syscalls (or for every
instruction), although this makes it VERY slow.  Useful to watch a program
build something.
