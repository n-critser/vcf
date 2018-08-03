/*
  TODO: use a linkedlist for email and phone (multiples)
  TODO: improve display - remove delimiters and protocol markers

  TODO: USE WARN AND ERR
#include <err.h>

void err(int eval, const char *fmt, ...);

void errx(int eval, const char *fmt, ...);

void warn(const char *fmt, ...);

void warnx(const char *fmt, ...);

#include <stdarg.h>

void verr(int eval, const char *fmt, va_list args);

void verrx(int eval, const char *fmt, va_list args);

void vwarn(const char *fmt, va_list args);

void vwarnx(const char *fmt, va_list args);

 */
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>
#include <err.h>

#include <errno.h>
#include "vcard.h"
//#include "eprintf.h"
#include "xutil.h"
#include "regex.h"


/* search related */
Cttree filterinorder(Cttree cttreep, Cttree  newtree,
		   int(*fn)(Cttree , void *), void *arg);
int fullsearchtree(Cttree p, void *arg);

/* int hamming_distance(unsigned x, unsigned y); */
/* int hamstring(char * newname, char * treename); */
/* int levenshtein_distance(const char *s, int len_s, const char*t, int len_t); */
/* const char * bitap_bitwise_search(const char *text, const char *pattern); */
struct cttree{
    char *name;
    char *fname;
    char *tel;
    char *email;
    char *addr;
    Cttree left;
    Cttree right;
};


Cttree insert (Cttree  cttreep, Cttree newctp)
{
    int cmp;
    if (cttreep == NULL){
	return newctp;
    }
    assert(cttreep);
    assert(cttreep->name);
    cmp = strcmp(newctp->name, cttreep->name);
    if (cmp == 0 ) {
	fprintf(stderr,"inset: duplicate entry %s ignored ",newctp->name);
    } else if (cmp < 0) {
	cttreep->left=insert (cttreep->left,newctp);
    } else {
	cttreep->right= insert(cttreep->right, newctp);
    }
    return cttreep;
}

/* weaksearch: returns a new Cttree of results
   based on the tree structure name field tree branchs are searched
   using possible regex match. 
*/
Cttree weaksearch(Cttree  cttreep, char*name, int *nct)
{
    *nct=0;
    Cttree  sres=NULL;
    Cttree temp = NULL;
    /* first loopup for a tree match*/
    if ((temp = lookup(cttreep, name, 1)) != NULL){
	sres = newitem(name,temp->fname,temp->email,temp->tel);
	(*nct)++;
    }

    if (!sres){
	/* search the tree using regex.h */
	sres = filterinorder(cttreep,sres,fullsearchtree,name);
    }
    return sres;
}

Cttree lookup (Cttree  cttreep, char *name, int search)
{
    int cmp ;
    if (cttreep == NULL){
	return NULL;
    }
    cmp = strcmp(name, cttreep->name);
    /* int hamdist=hamstring(name,cttreep->name); */
    /* char * treefound=strstr(name,cttreep->name); */
    /* char * namefound=strstr(cttreep->name,name); */
    if (cmp == 0){
	return cttreep;
    } else if( cmp < 0){
	return lookup(cttreep->left, name,search);
    } else {
	return lookup(cttreep->right, name,search);
    }
}

/* int hamstring(char * newname, char * treename) */
/* { */
/*     int newlen,treelen,counter,i,hamd; */
/*     hamd=0; */
/*     newlen=strlen(newname); */
/*     treelen=strlen(treename); */
/*     /\* set counter to smallest len*\/ */
/*     counter=newlen > treelen ? treelen : newlen; */
/*     for (i = 0 ; i < counter+1; i++){ */
/* 	hamd+=hamming_distance(newname[i],treename[i]); */
/*     } */
/*     /\* add the distance between string lengths to overall hammind distance *\/ */
/*     hamd+= (newlen - treelen)>=0? (newlen - treelen): (treelen - newlen); */
/*     return hamd; */
/* } */
/* /\* binary string hamming distance https://en.wikipedia.org/wiki/Hamming_distance *\/ */
/* int hamming_distance(unsigned x, unsigned y) */
/* { */
/*     int dist = 0; */
/*     unsigned  val = x ^ y; */

/*     // Count the number of bits set */
/*     while (val != 0) */
/*     { */
/*         // A bit is set, so increment the count and clear the bit */
/*         dist++; */
/*         val &= val - 1; */
/*     } */

/*     // Return the number of differing bits */
/*     return dist; */
/* } */

int minimum(int i, int j, int k)
{
    int min = (i < j)? i : j;
    min = (min < k) ? min: k;
    return min;
}
/*
  https://en.wikipedia.org/wiki/Levenshtein_distance
 */
/* int levenshtein_distance(const char *s, int len_s, const char*t, int len_t) */
/* { */
/*     int cost; */
/*     /\*base case*\/ */
/*     if (len_s == 0) return len_t; */
/*     if (len_t == 0) return len_s; */

/*     if (s[len_s-1] == t[len_t-1]){ */
/* 	cost = 0; */
/*     } else { */
/* 	cost = 1; */
/*     } */

/*     /\* recursion *\/ */
/*     return minimum(levenshtein_distance(s, len_s-1, t, len_t) +1, */
/* 		   levenshtein_distance(s, len_s, t, len_t -1) +1, */
/* 		   levenshtein_distance(s, len_s-1, t, len_t-1) +cost); */

/* } */

/*
  https://en.wikipedia.org/wiki/Bitap_algorithm
*/
/* const char * bitap_bitwise_search(const char *text, const char *pattern) */
/* { */
/*     int m = strlen(pattern); */
/*     unsigned long R; */
/*     unsigned long pattern_mask[CHAR_MAX+1]; */
/*     int i; */
/*     if (pattern[0] == '\0') return text; */
/*     if (m > 31){ */
/* 	printf ("PATTERN IS TOO LONG\n"); */
/* 	return NULL; */
/*     } */

/*     R = ~1; */

/*     for (i =0; i <= CHAR_MAX; ++i){ */
/* 	pattern_mask[i] = ~0; */
/*     } */

/*     for (i = 0; i < m; ++i){ */
/* 	pattern_mask[pattern[i]] &= ~(1UL << i); */
/*     } */

/*     for (i = 0; text[i] != '\0'; ++i){ */
/* 	R |= pattern_mask[text[i]]; */
/* 	R <<=1; */

/* 	if (0 == (R & (1UL << m))){ */
/* 	    return (text +i -m ) +1; */
/* 	} */
/*     } */
/*     return NULL; */
/* } */

void applyinorder(Cttree cttreep,
		  void(*fn)(Cttree , void *), void *arg)
{
    if (cttreep == NULL){
	return;
    }
    applyinorder(cttreep->left, fn, arg);
    (*fn)(cttreep, arg);
    applyinorder(cttreep->right,fn,arg);
}

void printcttree(Cttree p, void *arg)
{
    char *fmt;
    fmt = (char*) arg;
    printf(fmt , p->fname, p->name , p->email,p->tel);
}

Cttree  filterinorder(Cttree cttreep, Cttree newtree,
		  int(*fn)(Cttree , void *), void *arg)
{
    if (cttreep == NULL){
	return cttreep;
    }
    Cttree  temp;
    if ((temp=filterinorder(cttreep->left, newtree,fn, arg))){
	/* printf ("added left to newtree\n"); */
	newtree = temp;
    }

    if ((*fn)(cttreep, arg)){
	/* printf ("\nADDED NEW CONTACT TO FOUND LIST:%s\n",cttreep->name); */
	newtree=insert(newtree,newitem(xstrdup(cttreep->name),
			       xstrdup(cttreep->fname),
			       xstrdup(cttreep->email),
			       xstrdup(cttreep->tel)));
    }

    if ((temp = filterinorder(cttreep->right,newtree,fn,arg) )){
	/* printf ("added right to newtree\n"); */
	newtree= temp;
    }


    /* applyinorder(newtree,printcttree, */
    /* 		 "newtree\n>fname:%s\n name:%s \n email:%s\n tel:%s\n\n"); */

    return newtree;
}

char* strupper(const char * raw)
{
    char * newstr=xstrdup(raw);
    int i;
    i=0;
    for (i=0;newstr[i]!='\0';i++){
	newstr[i]=toupper(newstr[i]);
    }
    /* printf("raw: %s\n",raw); */
    /* printf("newstr: %s\n",newstr); */
    return newstr;
}

int fullsearchtree(Cttree p, void *arg)
{
    char *sterm;
    sterm= (char*)arg;
    char * upname=strupper(p->name);
    char * upsterm=strupper(sterm);
    int res = regf(upname,upsterm);
    free(upname);
    free(upsterm);
    return res;
}

Cttree newitem(char *name, char *fname, char* email, char * tel)
{
    Cttree newp;
    newp = (Cttree )xmalloc(sizeof(Cttree));
    newp->name=name;
    newp->fname=fname;
    newp->email=email;
    newp->tel = tel;
    /* printf ("new name: %s\n",name); */
    /* printf(" new fname: %s\n",fname); */
    newp->left=NULL;
    newp->right=NULL;
    assert(newp);
    return newp;
}

Cttree  vcfgetcontacts(FILE *f, int * count)
{
    char **cont=NULL;
    int maxline = 1;
    char buf[500];
    assert(f);
    Cttree  cttree = NULL;
    char **newct;
    int ncon,state,nline,found;
    if ((cont = (char**) malloc(maxline * sizeof(char*))) ==NULL){
	warnx("MALLOC FAILED- %s",strerror(errno) );
	return cttree;
    }
    nline= ncon = state = found =  0;
    int fnline ,nameline,telline,emailline;
    fnline = nameline=telline = emailline = 0;
    while (fgets (buf, sizeof(buf), f)!=NULL) {
	/* remove the \n added by fgets */
	buf[strlen(buf)-1]='\0';
	if (nline >= maxline-1){
	    maxline *=2;
	    printf ("maxline: %d", maxline);
	    if ((newct = (char**) realloc(cont, maxline* sizeof(char*)))
		== NULL){
		warnx("MALLOC FAILED- %s",strerror(errno) );
		return cttree;
	    }
	    cont = newct;
	}
	if (strncmp(buf,"BEGIN:VCARD",10) == 0){
	    state=1;
	    /*printf ("buf: '%s'\n", buf); */
	} else if(strncmp(buf,"END:VCARD",8)!=0 && state ==1){
	    /* contact line handling  */
	    if ((cont[nline]=xstrdup(strip(buf,"FN:", &found))) && found){
		fnline=nline;
		/* printf ("fnline:%d\n  - fn:%s\n",fnline,cont[fnline]); */
	    } else if((cont[nline]=xstrdup(strip(buf,
						"N:", &found))) && found){
		nameline=nline;
	    }else if((cont[nline]=xstrdup(strip(buf,
					       "EMAIL", &found))) && found){
		emailline=nline;
	    }else if((cont[nline]=xstrdup(strip(buf,
					       "TEL;", &found))) && found){
		telline=nline;
	    }else {
		cont[nline] = xstrdup(buf);
		/* printf("unparsed line : %s\n",buf); */
	    }
	    //printf ("fnline:%d  - fn:'%s'",fnline,cont[fnline]);
	    //printf ("found: %d\n",found);
	    //printf("line: %d - %s\n", nline, cont[nline]);
	    nline++;
	}else if (strncmp(buf,"END:VCARD",8)==0) {
	    if (cont[fnline]){
		printf("attempting insert: %s\n",cont[fnline]);
		cttree= insert(cttree,newitem(xstrdup(cont[nameline]),
					      xstrdup(cont[fnline]),
					      xstrdup(cont[emailline]),
					      xstrdup(cont[telline])));
	    }
	    int i=0;
	    for (i = 0; i < nline; i++){
		cont[i]="";
	    }
	    state = 0;
	    nline=0;
	    ncon++;
	    /* printf("\n********************\n"); */
	}
    }
    printf ("ncon: %d\n", ncon);
    *count=ncon;
    //p = xstrdup(buf);
    return cttree;
}


/* strip: strip prefix from tag and remove trailing \n left from fgets */
/* can i manipulate the tag and just return an int if the prefix is found*/
/*
>fname:Mike z. Target
 name:z. Target;Mike;;;
 email:CELL;PREF:3473000178
 tel:CELL;PREF:3473000178


 */
char * strip(char* tag, char * prefix, int *found)
{
    int i;
    if (strncmp(tag,prefix,strlen(prefix)) == 0){
	//tag=strchr(tag,':');
	for(i=strlen(tag) ; i >=0 && !isalpha(tag[i]); i-- ){
	    /* if (tag[i] == ';' && tag[i+1] != ';'){ */
	    /* 	tag[i] = '-'; */

	    /* } else if (tag[i] == ';'){ */
	    /* 	/\* remove the semicolons delimiting some words*\/ */
	    /* 	memmove(tag+i, tag+i+1, strlen(tag+i+1)); /\**\/ */
	    /* } */
	    if (tag[i] == ';'){
		memmove(tag+i, tag+i+1,strlen(tag+i)+1);
	    }
	}
	tag +=strlen(prefix);
	*found = 1;
    }else {
	*found =0;
    }
    return tag;
}

