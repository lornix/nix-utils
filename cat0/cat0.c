/* cat0 - convert CRLF to 0 bytes */

/* L R Nix <lornix@lornix.com> */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* read files or stdin, covert CRLF endings to 0x00 bytes, allowing input
   to be used with 'xargs -0', 'grep -0' and others
 */

/* REPLACECHAR is output in place of CRLF (\n) chars */
#define REPLACECHAR 0

/* size of static buffer in readfd function */
#define BUFFSIZE 1024

void readfd(int fd)
{
    static unsigned char mybuffer[BUFFSIZE];
    int numread;
    /* read until 0 bytes returned, implying EOF */
    do {
        numread=read(fd,mybuffer,BUFFSIZE);
        if (numread>0) {
            int i;
            for (i=0; i<numread; i++) {
                if (mybuffer[i]=='\n') {
                    mybuffer[i]=REPLACECHAR;
                }
                /* output whatever to stdout */
                putchar(mybuffer[i]);
            }
        }
    } while (numread>0);
}

int main(int argc,char *argv[])
{
    int fd=0;
    int filecnt=1;
    /* if there are filenames given on cmdline, read & output them in order */
    while (filecnt<argc) {
        /* set up to read from file, put fd# in fd */
        fd=open(argv[filecnt],O_RDONLY);
        if (fd>=0) {
            readfd(fd);
            close(fd);
        }
        filecnt++;
    }
    /* Otherwise, read & output STDIN until EOF */
    if (argc<2) {
        /* set up to read STDIN (fd=0) */
        fd=0;
        readfd(fd);
    }
    return 0;
}
