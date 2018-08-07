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
    Cttree left;
    Cttree right;
    char *name;
    char *fname;
    char *tel;
    char *email;
    char *addr;
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
    printf("free fname\n");
    free(cttreep->fname);
    printf("free name\n");
    free(cttreep->name);
    printf("free tel\n");
    free(cttreep->tel);
    if (cttreep->addr){
	printf("free addr\n");
	free(cttreep->addr);
    }
    if (cttreep->email){
	printf("free email\n");
	free(cttreep->email);
    }
    printf("free cttreep\n");
    free(cttreep);
}

void
rec_destroy(Cttree cttreep)
{

    if (cttreep==NULL){
	return;
    }
    printf("destroying cttreep->name:%s\n",cttreep->name);
    /* printf("destroying cttreep->left->name:%s\n",cttreep->left->name); */
    /* printf("destroying cttreep->right->name:%s\n",cttreep->right->name); */
    rec_destroy(cttreep->left);
    rec_destroy(cttreep->right);

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
    printf ("INSERT!\n");
    if (cttreep == NULL){
	printf ("newp->depth: %d\n",newctp->depth);
	printf ("NULL\n");
	newctp->left =NULL;
	newctp->right = NULL;
	return newctp;
    }
    printf ("cttreep->name: %s - cttreep->depth: %d\n",cttreep->name,cttreep->depth);
    printf ("newp->name: %s - newp->depth: %d\n",newctp->name,newctp->depth);
    assert(cttreep);
    assert(cttreep->name);
    printf ("strlen(cttreep->name): %lu\n", strlen(cttreep->name));
    printf("rname: %s  \n",cttreep->name);
    printf("nname: %s  \n",newctp->name);
    cmp = strcmp(newctp->name, cttreep->name);
    printf("cmp: %d\n",cmp);
    if (cmp == 0 ) {
	fprintf(stderr,"insert: duplicate entry %s ignored ",newctp->name);
    } else if (cmp < 0) {
	printf("left\n");
	newctp->depth+=1;
	cttreep->left=insert (cttreep->left,newctp);
    } else {
	printf("right\n");
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
    printf("cttreep->name:%s\n", cttreep->name);
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
	printf ("added left to newtree\n");
	newtree = temp;
    }

    if ((*fn)(cttreep, arg)){
	printf ("\nADDED NEW CONTACT TO FOUND LIST:%s\n",cttreep->name);
	Cttree newp = (Cttree )xmalloc(sizeof(Cttree));
	newp->name=xstrdup(cttreep->name);
	newp->fname=xstrdup(cttreep->fname);
	newp->email=xstrdup(cttreep->email);
	newp->tel = xstrdup(cttreep->tel);
	newp->left=NULL;
	newp->right=NULL;
	assert(newp->name == cttreep->name);
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

Cttree newitem(const char *name, const char *fname,
	       const char* email, const char * tel)
{
    Cttree newp;
    newp = (Cttree )xmalloc(sizeof(Cttree));
    newp->name=xstrdup(name);
    newp->fname=xstrdup(fname);
    newp->email=xstrdup(email);
    newp->tel = xstrdup(tel);
    newp->left=NULL;
    newp->right=NULL;
    assert(newp);
    printf("name: %s -- newp->name:%s \n",name,newp->name);
    assert(strlen(name) == strlen(newp->name));
    assert(strlen(fname) == strlen(newp->name));
    return newp;
}

Cttree newemptyitem()
{
    Cttree newp;
    newp = (Cttree )xmalloc(sizeof(Cttree));
    newp->depth=0;
    newp->left=NULL;
    newp->right=NULL;
    newp->name="EMPTY";
    newp->fname="EMPTY";
    newp->email="EMPTY";
    newp->tel="EMPTY";
    newp->addr="EMPTY";
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
	    printf("creating newp2 \n");
	    newp2 =newemptyitem();//(Cttree )xmalloc(sizeof(Cttree));
	    assert(newp2);
	    printf("BEGIN : newp2->depth: %d\n", newp2->depth);
	    /*printf ("buf: '%s'\n", buf); */
	} else if(strncmp(buf,"END:VCARD",8)!=0 && state ==1){
	    /* contact line handling  */
	    if ((line = xstrdup(strip(buf,"FN:", &found))) && found){
		newp2->fname = line;
		printf("FN : newp2->depth: %d\n", newp2->depth);
		/* printf ("fnline:%d\n  - fn:%s\n",fnline,cont[fnline]); */
	    } else if((line = xstrdup(strip(buf,"N:", &found))) && found){
		printf("buf:%s\n", buf);
		printf("N : newp2->depth: %d\n", newp2->depth);
		newp2->name = line ;
	    }else if((line = xstrdup(strip(buf,"EMAIL", &found))) && found){
		newp2->email = line;
	    }else if((line=xstrdup(strip(buf,"TEL;", &found))) && found){
		newp2->tel = line;
	    }else {
		continue;
	    }
	    nline++;
	    printf ("nline:%d\n",nline);
	}else if (strncmp(buf,"END:VCARD",8)==0) {
	    if (newp2){
		/* create an item to be inserted into tree */
		/* Cttree next = newitem(cont[nameline], */
		/* 		      cont[fnline], */
		/* 		      cont[emailline], */
		/* 		      cont[telline]); */

		printf("newp2->name: %s - newp2->depth: %d \n",
		       newp2->name,newp2->depth  );
		cttreep= insert(cttreep,newp2);
		newp2=NULL;

	    }
	    state = 0;
	    nline=0;
	    ncon++;
	    printf("\n********************\n");
	}
    }
    printf ("ncon: %d\n", ncon);
    *count=ncon;
    /* show the current tree */
    applyinorder(cttreep,printcttree,
    		 "CTS\n>fname:%s\n name:%s \n email:%s\n tel:%s\n\n");

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
	    printf("new string? : %s\n", new);
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

