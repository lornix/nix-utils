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

void showfloat(float f)
{
    union {
	float f;
	long int l;
    } x;
    x.f=f;
    /* ========== bits: 1 8 23 = sign,exponent,mantissa */
    printf("%08lx (%lx-%02lx-%06lx)",x.l,(x.l>>31)&1,(x.l>>23)&((1l<<8)-1),x.l&((1l<<23)-1));
    // 3fb4fdf4 / 0-7f-34fdf4 = 1.414 : 1.414000
    // 0 : 0111-1111 : 011-0100-1111-1101-1111-0100 (1/8/23)
}

#ifdef __x86_64
/* only add this if we're compiling on 64bit machine */
void showdouble(double d)
{
    union {
	double d;
	unsigned long int l;
    } x;
    x.d=d;
    /* =============== bits: 1 11 52 = sign,exponent,mantissa */
    printf("%016lx (%lx-%03lx-%013lx)",x.l,((x.l>>63)&1),(x.l>>52)&((1l<<11)-1),(x.l&((1l<<52)-1)));
    // 3ff69fbe76c8b439 / 0-3ff-69fbe76c8b439 = 1.414 : 1.414000
    // 0 : 011-1111-1111 : 0110-1001-1111-1011-1110-0111-0110-1100-1000-1011-0100-0011-1001 (1/11/52)
}
#endif
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

void showresults(int f1,int f2,int f3,int f4,char*s)
{
    double d=strtod(s,NULL);
    float f=strtof(s,NULL);
    unsigned long int i=strtoul(s,NULL,0);
    int padlen;
    printf("%*s",f1,s);
    printf(" => ");
    printf("%*f",f4,f);
    printf(" : ");
    printf("%*a",f3,d);
    printf(" : ");
    printf("0x");
    if (i>=(1ul<<32)) {
        printf("%0*lx",(f2/2),i>>32);
        printf(" ");
        printf("%0*lx",(f2/2),i&0xffffffff);
    }
    else {
        printf("%0*lx",f2,i);
    }
    padlen=f1;
    printf("\n");
    printf("%*s => ",padlen," ");
    showfloat(f);
#ifdef __x86_64
/* only add this if we're compiling on 64bit machine */
    printf(" : ");
    showdouble(d);
#endif
    printf("\n");
    printf("%*s => ",padlen," ");
    showbinary(d);
    printf("\n");
}

int main(int argc,char*argv[])
{
    int cnt;
    unsigned int tmp,f1len=1,f2len=1,f3len=1,f4len=1;
#define MAXSTRLEN 100
    char tmpstr[MAXSTRLEN];


#ifndef TEST
    if (argc<2) {
	printf("need a number\n");
	exit(1);
    }
#endif

    /* determine max length of all fields */
    cnt=0;
    while ((++cnt)<argc) {
	if (strlen(argv[cnt])>f1len)
	    f1len=strlen(argv[cnt]);
	if ((tmp=snprintf(tmpstr,MAXSTRLEN,"%lx",strtoul(argv[cnt],NULL,0)))>f2len)
	    f2len=tmp;
	if ((tmp=snprintf(tmpstr,MAXSTRLEN,"%a",strtod(argv[cnt],NULL)))>f3len)
	    f3len=tmp;
	if ((tmp=snprintf(tmpstr,MAXSTRLEN,"%f",strtod(argv[cnt],NULL)))>f4len)
	    f4len=tmp;
    }
    /* round f2len up to next higher length (2/4/8/16) */
    if (f2len>8) f2len=16;
    else if (f2len>4) f2len=8;
    else if (f2len>2) f2len=4;
    else f2len=2;

    /* now display the arguments, using field widths from above */
    cnt=0;
    while ((++cnt)<argc) {
	showresults(f1len,f2len,f3len,f4len,argv[cnt]);
    }

#ifdef TEST
#define SHORT_PI "3.14"

    showresults(4,2,20,8,SHORT_PI);

    printf("EXPECTED:\n");
    printf("3.14 => 0x03 : 0x1.91eb851eb851fp+1 : 3.140000 : 4048f5c3 (0-80-48f5c3) : 40091eb851eb851f (0-400-91eb851eb851f)\n");
    printf("             : 11.001000111101011100001010001111010111000010100011111\n");

#endif

    return 0;
}
