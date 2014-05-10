/***********************************************************************/
/* showfloat - display hex versions of given number in single & double */
/*             precision.                                              */
/*                                                                     */
/* ./showfloat 3.1415926535                                            */
/* 40490fdb / 0-80-490fdb : 400921fb54411744 / 0-400-921fb54411744 =   */
/*                     0x1.921fb54411744p+1 : 3.1415926535 : 3.141593  */
/* (output wrapped)                                                    */
/*                                                                     */
/* loni nix <lornix@lornix.com>                                        */
/* October 31 2010                                                     */
/***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * Some assumptions:
 *    sizeof(float) = 4 bytes
 *    sizeof(double) = 8 bytes
 *    little endian system
 */

void showbinary(double d)
{
#define MAXDIGITS 100
    char digits[MAXDIGITS];
    int currentdigit=0;
    unsigned long int i;
    /* we don't handle negative's just yet */
    /* besides, we want binary representation, not 2-comp version */
    if (d<0.0) {
	putchar('-');
	d=-d;
    }
    /* display the integer part */
    i=(unsigned long int)d;
    do {
	digits[currentdigit]=0x30|(i&1);
	i/=2l;
	currentdigit++;
	if (currentdigit>MAXDIGITS) {
	    putchar('*');
	    break;
	}
    } while (i>0);
    currentdigit--;
    while (currentdigit>=0) {
        putchar(digits[currentdigit--]);
    }
    putchar('.');
    /* obtain decimal part */
    d=d-floor(d);
    while (d>0.0) {
	d=d*2.0;
	if (d>=1.0) {
	    putchar('1');
	    d=d-1.0;
	} else {
	    putchar('0');
	}
    }
}

void showresults(int arglen,char*s)
{
    double d=strtod(s,NULL);
    float f=strtof(s,NULL);
    unsigned long int i=strtoul(s,NULL,0);
    union {
	float f;
	double d;
	unsigned long int l;
    } x;

    printf("%*s",arglen,s);
    printf(" => ");
    printf("(f) %f",f);
    printf(" : ");
    printf("(d) %f",d);
    printf(" : ");
    printf("(da) %a",d);
    printf(" : ");
    printf("(ix) 0x");
    if (i>=(1ul<<32)) {
        printf("%0lx",i>>32);
        printf(" ");
        printf("%0lx",i&0xffffffff);
    }
    else {
        printf("%0lx",i);
    }
    printf("\n");

    printf("%*s => ",arglen," ");

    x.l=0; /* clear out union */
    x.f=f;
    /* ========== bits: 1 8 23 = sign,exponent,mantissa */
    printf("(f) %08lx (%lx-%02lx-%06lx)",x.l,(x.l>>31)&1,(x.l>>23)&((1l<<8)-1),x.l&((1l<<23)-1));

    printf(" : ");

    x.l=0; /* clear out union */
    x.d=d;
    /* =============== bits: 1 11 52 = sign,exponent,mantissa */
    printf("(d) %016lx (%lx-%03lx-%013lx)",x.l,((x.l>>63)&1),(x.l>>52)&((1l<<11)-1),(x.l&((1l<<52)-1)));

    printf("\n");

    printf("%*s => ",arglen," ");

    showbinary(d);

    printf("\n");
}

int main(int argc,char*argv[])
{
    int cnt;
    unsigned int arglen=1;

#ifndef TEST
    if (argc<2) {
	printf("need a number\n");
	exit(1);
    }
#endif

    /* determine max length of all fields */
    cnt=0;
    while ((++cnt)<argc) {
	if (strlen(argv[cnt])>arglen)
	    arglen=strlen(argv[cnt]);
    }

    /* now display the arguments, using field widths from above */
    cnt=0;
    while ((++cnt)<argc) {
	showresults(arglen,argv[cnt]);
    }

    return 0;
}
