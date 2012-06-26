#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc,char*argv[])
{
    int current=1;
    long int errornum;
    while (current<argc) {
        errornum=strtol(argv[current],NULL,0);
        /* we might receive a negative value, invert sign */
        if (errornum<0) {
            errornum=-errornum;
        }
        printf("%ld: %s\n",errornum,strerror(errornum));
        current++;
    }
    return 0;
}
