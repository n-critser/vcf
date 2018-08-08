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
#include <strings.h>
#include <errno.h>
#include "vcard.h"
//#include "eprintf.h"
#include "xutil.h"
#include "regex.h"


Cttree newemptyitem();


/* search related */
Cttree filterinorder(Cttree cttreep, Cttree  newtree,
		   int(*fn)(Cttree , void *), void *arg);
int fullsearchtree(Cttree p, void *arg);


struct cttree{
    int depth;
    char *name;
    char *fname;
    char *tel;
    char *email;
    char *addr;
    Cttree left;
    Cttree right;

};


void counttree(Cttree p, void*arg)
{
    int *intp;
    intp = (int *) arg;
    (*intp)++;
}

void depthtree(Cttree p, void*arg)
{
    int *intp;
    intp = (int *) arg;
    if (p->depth > (*intp)){
	(*intp) = p->depth;
    }
}


Vcf
construct()
{
    Vcf v;
    v = xmalloc(sizeof(*v));
    v->tree=NULL;
    v->count = 0;
    return v;
}

void
freeall(Cttree cttreep){
    if (cttreep == NULL){
	return;
    }
    /* printf("free fname\n"); */
    assert(cttreep->fname);
    free(cttreep->fname);
    /* printf("free name\n"); */
    assert(cttreep->name);
    free(cttreep->name);
    /* printf("free tel\n"); */
    if (cttreep->tel){
	assert(cttreep->tel);
	free(cttreep->tel);
    }
    if (cttreep->addr){
	/* printf("free addr\n"); */
	assert(cttreep->addr);
	free(cttreep->addr);
    }
    if (cttreep->email){
	/* printf("free email\n"); */
	assert(cttreep->email);
	free(cttreep->email);
    }
    /* printf("free cttreep\n"); */
    assert(cttreep);
    free(cttreep);
}

void
rec_destroy(Cttree cttreep)
{

    if (cttreep==NULL){
	return;
    }

    rec_destroy(cttreep->left);
    rec_destroy(cttreep->right);
    /* printf("destroying cttreep->name:%s\n",cttreep->name); */
    freeall(cttreep);
}

void
destroy(Vcf vcfp)
{
    if (vcfp== NULL){
	return;
    }
    Cttree cttreep =vcfp->tree;
    rec_destroy(cttreep);
    /* destroy(cttreep->left); */
    /* destroy(cttreep->right); */
    /* freeall(cttreep); */
}


Cttree insert (Cttree  cttreep, Cttree newctp)
{
    int cmp;
    assert(newctp);
    /* printf ("INSERT!\n"); */
    if (cttreep == NULL){
	/* printf ("newp->depth: %d\n",newctp->depth); */
	newctp->left =NULL;
	newctp->right = NULL;
	return newctp;
    }
    assert(cttreep);
    assert(cttreep->name);

    if (!newctp->name){
	newctp->name=xstrdup(newctp->fname);
    }
    assert(newctp->name);
    cmp = strcmp(newctp->name, cttreep->name);
    if (cmp == 0 ) {
	fprintf(stderr,"insert: duplicate entry %s ignored ",newctp->name);
    } else if (cmp < 0) {
	newctp->depth+=1;
	cttreep->left=insert (cttreep->left,newctp);
    } else {
	newctp->depth+=1;
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
	sres = newitem(name,temp->fname,temp->email,temp->tel,temp->addr);
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
    if (cmp == 0){
	return cttreep;
    } else if( cmp < 0){
	return lookup(cttreep->left, name,search);
    } else {
	return lookup(cttreep->right, name,search);
    }
}


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
    assert(cttreep);
    Cttree  temp = NULL;

    if ((temp=filterinorder(cttreep->left, newtree,fn, arg))){
	/* printf ("added left to newtree\n"); */
	newtree = temp;
    }

    if ((*fn)(cttreep, arg)){
	printf ("\nADDED NEW CONTACT TO FOUND LIST:%s\n",cttreep->name);
	Cttree newp = newitem(cttreep->name,
			      cttreep->fname,
			      cttreep->email,
			      cttreep->tel,
			      cttreep->addr
			      );
	assert(strcmp(newp->name, cttreep->name) == 0);
 	newtree=insert(newtree,newp);
	newp=NULL;
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


enum {
    NFIELDS = 5
};
const char * fields[NFIELDS] ={
    "name",
    "fname",
    "email",
    "tel",
    "addr"
};

Cttree newitem(const char *name, const char *fname,
	       const char* email, const char * tel, const char *addr)
{
    Cttree newp;
    newp = (Cttree )xmalloc(sizeof(struct cttree));
    /* int i; */
    /* for (i = 0; i < NFIELDS; i++){ */
    /* 	newp->fields[i] = xstrdup(fields[i]); */
    /* } */
    newp->name=name ? xstrdup(name): NULL ;
    newp->fname=fname ? xstrdup(fname): NULL ;
    newp->email=email ? xstrdup(email) : NULL ;
    newp->tel = tel ? xstrdup(tel) : NULL ;
    newp->addr=addr ? xstrdup(addr) : NULL ;
    newp->left=NULL;
    newp->right=NULL;
    newp->depth = 0;
    assert(newp);
    assert(strlen(name) == strlen(newp->name));
    assert(strlen(fname) == strlen(newp->fname));
    return newp;
}

Cttree newemptyitem()
{
    Cttree newp;
    newp = (Cttree )xmalloc(sizeof(struct cttree));
    newp->depth=0;
    newp->left=NULL;
    newp->right=NULL;
    newp->name=NULL;
    newp->fname=NULL;
    newp->email=NULL;
    newp->tel=NULL;
    newp->addr=NULL;
    assert(newp);
    assert(newp->depth ==0);
    return newp;
}


Cttree  vcfgetcontacts(Vcf vcfp, FILE *f, int * count)
{
    char buf[500];
    assert(f);
    Cttree  cttreep = vcfp->tree;
    int ncon,state,nline,found;
    nline= ncon = state = found =  0;
    Cttree newp2;
    char *line;
    while (fgets (buf, sizeof(buf), f)!=NULL) {
	/* printf ("strlen(buf): %lu\n",strlen(buf)); */
	/* remove the \n added by fgets */
	buf[strlen(buf)-1]='\0';
	line=NULL;
	assert(buf);
	if (strncmp(buf,"BEGIN:VCARD",10) == 0){
	    state=1;
	    /* memory for the next contact */
	    /* printf("creating newp2 \n"); */
	    newp2 =newemptyitem();//(Cttree )xmalloc(sizeof(Cttree));
	    assert(newp2);
	} else if(strncmp(buf,"END:VCARD",8)!=0 && state ==1){
	    /* contact line handling  */
	    if ((line = xstrdup(strip(buf,"FN:", &found))) && found){
		newp2->fname = line;
		/* printf("FN : newp2->depth: %d\n", newp2->depth); */

	    } else if((line = xstrdup(strip(buf,"N:", &found))) && found){
		/* printf("buf:%s\n", buf); */
		/* printf("N : newp2->depth: %d\n", newp2->depth); */
		newp2->name = line ;
	    }else if((line = xstrdup(strip(buf,"EMAIL", &found))) && found){
		/* concatenation code taken from the
		   below stackoverflow answers
		   https://stackoverflow.com/questions/8465006/
		   how-do-i-concatenate-two-strings-in-c
		 */
		/* concatenate if we already have one*/
		if (newp2->email){
		    const size_t len1 = strlen(newp2->email);
		    const size_t len2 = strlen(line);
		    char *result = xmalloc(len1 + len2 + 1); // +1 for the null-terminator
		    // in real code you would check for errors in malloc here
		    memcpy(result, newp2->email, len1);
		    memcpy(result+(len1), "\n", 1);
		    memcpy(result + (len1+1), line, len2 + 1); // +1 to copy the null-terminator
		    free(line);
		    free(newp2->email);
		    newp2->email=result;
		}else {
		    newp2->email = line;
		}
	    }else if((line=xstrdup(strip(buf,"TEL;", &found))) && found){
		if (newp2->tel){
		    const size_t len1 = strlen(newp2->tel);
		    const size_t len2 = strlen(line);
		    char *result = xmalloc(len1 + len2 + 1); // +1 for the null-terminator
		    // in real code you would check for errors in malloc here
		    memcpy(result, newp2->tel, len1);
		    memcpy(result+(len1), "\n", 1);
		    memcpy(result + (len1+1), line, len2 + 1); // +1 to copy the null-terminator
		    free(line);
		    free(newp2->tel);
		    newp2->tel=result;

		}else {
		    newp2->tel = line;
		}
	    }else {
		continue;
	    }
	    nline++;
	}else if (strncmp(buf,"END:VCARD",8)==0) {

	    if (newp2){
		char  tmpnm[1024];
		int res= sprintf(tmpnm,"%d", ncon );
		/* printf ("tmpnm : %s\n", tmpnm); */
		if (!newp2->fname ){
		    newp2->fname= xstrdup(tmpnm);
		}
		if (!newp2->name ){
		    newp2->name= xstrdup(tmpnm);
		}
		cttreep= insert(cttreep,newp2);
		newp2=NULL;

	    }
	    state = 0;
	    nline=0;
	    ncon++;
	    /* printf("\n********************\n"); */
	}
    }

    *count=ncon;
    /* show the current tree */
    /* applyinorder(cttreep,printcttree, */
    /* 		 "CTS\n>fname:%s\n name:%s \n email:%s\n tel:%s\n\n"); */
    /* printf ("ncon: %d\n", ncon); */
    return cttreep;
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
    char *new;
    if (strncmp(tag,prefix,strlen(prefix)) == 0){
	//tag=strchr(tag,':');
	new = rindex(tag,':');
	new++;
	if (new){
	    /* printf("new string? : %s\n", new); */
	    tag = new;
	} else {
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
	}
	*found = 1;
    }else {
	*found =0;
    }
    return tag;
}

