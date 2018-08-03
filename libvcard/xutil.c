#include "xutil.h"
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <errno.h>

static char * progname=NULL;

void * xmalloc(size_t size)
{
    void *p;
    if (!size){
	fprintf(stderr, "CAN'T MALLOC 0");
	exit(1);
    }
    if ((p = malloc(size)) == NULL){
	fprintf(stderr,
		"MALLOC FAILED %zu bytes - error: %s", size,strerror(errno));
	exit(1);
    }
    return p;
}

char * xstrdup(const char* str )
{
    char *ret;
    size_t slen = strlen(str)+1;
    ret = xmalloc(slen);
    strncpy(ret,str,slen);
    return ret;
}


void setprogname(const char *pn)
{
    progname =xstrdup(pn);
}

char * getprogname(void)
{
    return progname;
}
