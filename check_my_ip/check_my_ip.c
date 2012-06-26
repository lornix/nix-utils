#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <regex.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <curl/curl.h>

#define ROUTER_STATUS_URL   "http://192.168.0.1"
#define EXT_IP_REGEX	    ">IP Address<[^0-9.]*\\([0-9.]*\\)<"
#define HOSTNAME_LOOKUP	    "lornix.com"

struct info_s
{
    void *rdata;	    /* pointer to received data */
    unsigned int rlength;   /* how many bytes stored so far */
    void *sdata;	    /* pointer to sent data */
    unsigned int slength;   /* how many bytes stored so far */
#define IPSTR_SIZE 40
    char ipstr[IPSTR_SIZE+1]; /* storage for modem address (ascii) */
    char lustr[IPSTR_SIZE+1]; /* storage for lookup address (ascii) */
};

size_t receive_data(void *ptr,size_t size,size_t num,struct info_s *userdata)
{
    /* callback function for curl GET action */
    unsigned int length=size*num;
    /* did we get any data to store? */
    if (length>0)
    {
	/* determine the new length */
	unsigned int newlen=userdata->rlength+length;
	/* and resize the memory chunk as needed */
	userdata->rdata=realloc(userdata->rdata,newlen+1);
	/* resize successful? */
	if (userdata->rdata!=NULL)
	{
	    /* copy the new data into the end of chunk */
	    memcpy(userdata->rdata+userdata->rlength,ptr,length);
	    /* force it to be zero-terminated */
	    memset(userdata->rdata+newlen,0,1);
	    /* update length storage */
	    userdata->rlength=newlen;
	}
	else
	{
	    /* something broke during memory allocation */
	    fprintf(stderr,"error allocating memory during receive\n");
	    length=0;
	}
    }
    /* if length!=(num*size), curl routine aborts */
    return length;
}

int get_page(struct info_s *info,char *url)
{
    /* pointer to CURL handle */
    CURL *pcurl;
    /* storage for error messages if needed */
    char curlerr[CURL_ERROR_SIZE+1];
    /* so we can trap errors */
    int err;

    /* init CURL routines */
    curl_global_init(CURL_GLOBAL_ALL);
    /* we're using the 'easy' routines (ha!) */
    pcurl=curl_easy_init();
    /* error? */
    if (pcurl==NULL)
    {
	/* tell operator, set an error state, finish */
	fprintf(stderr,"Error initializing libcurl\n");
	err=1;
	goto leave;
    }
    /* set up various options to cause GET of url */
    curl_easy_setopt(pcurl,CURLOPT_ERRORBUFFER,curlerr);
    curl_easy_setopt(pcurl,CURLOPT_VERBOSE,0);
    curl_easy_setopt(pcurl,CURLOPT_MAXREDIRS,5);
    curl_easy_setopt(pcurl,CURLOPT_TIMEOUT,5);
    curl_easy_setopt(pcurl,CURLOPT_WRITEDATA,info);
    curl_easy_setopt(pcurl,CURLOPT_WRITEFUNCTION,receive_data);
    curl_easy_setopt(pcurl,CURLOPT_URL,url);

    /* now actually DO the work */
    err=curl_easy_perform(pcurl);
    /* error? */
    if (err!=0)
    {
	/* use curlerr to give error message too */
	fprintf(stderr,"%s\n",curlerr);
	goto leave;
    }
    /* de-init CURL, easy & global */
    curl_easy_cleanup(pcurl);
    curl_global_cleanup();
    /* printf("%d bytes received\n",info->rlength); */
leave:
    /* the usual, 0=good, !0=bad */
    return err;
}

int search_for(struct info_s *info,const char const *searchfor)
{
    int err;
    regex_t pregex;
    int nummatches=5;
    regmatch_t regmatch[nummatches];
    int match;

    /* perform regex to find IP addr */
    err=regcomp(&pregex,searchfor,REG_ICASE);
    if (err!=0)
    {
	fprintf(stderr,"regex compile error %d\n",err);
	goto leave;
    }
    err=regexec(&pregex,info->rdata,nummatches,regmatch,0);
    switch (err)
    {
	case 0:
	    for (match=0; match<nummatches; match++)
	    {
		if (regmatch[match].rm_so>=0)
		{
		    memset(info->ipstr,0,IPSTR_SIZE+1);
		    memcpy(info->ipstr,info->rdata+regmatch[match].rm_so,
			    ((regmatch[match].rm_eo-regmatch[match].rm_so)<IPSTR_SIZE)?
			    (regmatch[match].rm_eo-regmatch[match].rm_so):IPSTR_SIZE);
		    /* printf("Match #%d: %d, %d = '%s'\n", */
		    /*         match,                       */
		    /*         regmatch[match].rm_so,       */
		    /*         regmatch[match].rm_eo,       */
		    /*         info.ipstr);                 */
		}
	    }
	    break;
	case 1:
	    fprintf(stderr,"IP address not found, exiting.\n");
	    break;
	default:
	    {
#define ERRBUF_SIZE 254
		char errbuf[ERRBUF_SIZE+1];
		regerror(err,&pregex,errbuf,ERRBUF_SIZE);
		fprintf(stderr,"regex exec error %d: %s\n",err,errbuf);
	    }
	    break;
    }
leave:
    regfree(&pregex);
    /* if (err==0) */
    /*     printf("Current IP Addr: %s\n",info->ipstr); */
    return err;
}

int lookup_ip(struct info_s *info,char *hname)
{
    int err;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *item;

    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family=AF_INET;
    hints.ai_flags=AI_ADDRCONFIG;

    err=getaddrinfo(hname,NULL,&hints,&res);
    if (err!=0)
    {
	fprintf(stderr,"Lookup IP: %s\n",gai_strerror(err));
	goto leave;
    }
    item=res;
    while (item!=NULL)
    {
	struct sockaddr_in *sin=(struct sockaddr_in *)item->ai_addr;
	struct in_addr ina=sin->sin_addr;
	inet_ntop(item->ai_family,&ina,info->lustr,IPSTR_SIZE);
	/* printf("DNS lookup: %s\n",info->lustr); */
	item=item->ai_next;
	item=NULL; /* short-circuit, only need first addr */
    }
leave:
    freeaddrinfo(res);
    return err;
}

int main()
{
    int err;
    struct info_s info;

    /* preset values in structure to start fresh */
    info.rdata=NULL;
    info.rlength=0;
    info.sdata=NULL;
    info.slength=0;

    err=get_page(&info,ROUTER_STATUS_URL);
    if (err!=0)
	goto leave;
    /* printf("%s",(char *)info.rdata); */

    err=search_for(&info,EXT_IP_REGEX);
    if (err!=0)
	goto leave;

    err=lookup_ip(&info,HOSTNAME_LOOKUP);
    if (err!=0)
	goto leave;

    printf("%s ?== %s : ",info.ipstr,info.lustr);
    printf("%s\n",(strncmp(info.ipstr,info.lustr,IPSTR_SIZE)==0)?"Yes":"No");

leave:
    if (info.rdata!=NULL) free(info.rdata);
    if (info.sdata!=NULL) free(info.sdata);
    return err;
}
