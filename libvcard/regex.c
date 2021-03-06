#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "regex.h"
#include "xutil.h"

/*regf: return a xstrdup copy of txt if pat found or NULL*/
int regf(const char * txt, const char * pat)
{
    STATE s = S0;
    /* printf ("regf:\t txt:%s \t pat:%s\n",txt,pat); */
    do{
	/* skip ahead in txt until we hit part of pat */
	if ((*txt) == (*pat)){
	    /* STATE goes to S1 */
	    s = S1;
	    pat++;
	    txt++;
	    break;
	}
	txt++;
    }while((*txt)!='\0');
    /* printf ("after initial search : regf:\t txt:%s \t pat:%s\n",txt,pat); */
    if (s != S0 && regwalk(txt,pat ) ){
	return 1;
    }
    return 0;
}

/* regwalk: recusively consume txt return 1 if pat accepts or 0 otherwise */
int regwalk(const char *txt,  const char *pat)
{
    /* basecase */
    if ((*pat) == '\0'){ /* exact string*/
	/* printf ("*******EXACT STRING*********\n"); */
	return 1;
    }
    if (*txt!='\0' && (pat[0] == *txt)){
	assert(txt);
	assert(pat);
	/* printf ("txt:%s - %c \n", txt,*txt); */
	/* printf ("pat:%s - %c  \n", pat,*pat); */
	return regwalk((txt+1),(pat+1));
    }
    /* printf ("FAILE SEARCH!\n"); */
    return 0;
}
