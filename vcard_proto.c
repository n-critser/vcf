#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../eprintf.h"

/* tree code adapted from practice of programming */
typedef struct Cttree Cttree;
struct Cttree{
    char *name;
    char *fname;
    Cttree *left;
    Cttree *right;
};

Cttree *insert (Cttree * cttreep, Cttree *newctp)
{
    int cmp;
    if (cttreep == NULL){
	return newctp;
    }
    cmp = strcmp(newctp->name, cttreep->name);
    if (cmp == 0 ) {
	weprintf("inset: duplicate entry %s ignored ",newctp->name);
    } else if (cmp < 0) {
	cttreep->left=insert (cttreep->left,newctp);
    } else {
	cttreep->right= insert(cttreep->right, newctp);
    }
    return cttreep;
}


Cttree *lookup (Cttree * cttreep, char *name)
{
    int cmp ;
    if (cttreep == NULL){
	return NULL;
    }
    cmp = strcmp(name, cttreep->name);
    if (cmp == 0){
	return cttreep;
    } else if( cmp < 0){
	return lookup(cttreep->left, name);
    } else {
	return lookup(cttreep->right, name);
    }
}

void applyinorder(Cttree *cttreep,
		  void(*fn)(Cttree*, void *), void *arg)
{
    if (cttreep == NULL){
	return;
    }
    applyinorder(cttreep->left, fn, arg);
    (*fn)(cttreep, arg);
    applyinorder(cttreep->right,fn,arg);
}

void printcttree(Cttree *p, void *arg)
{
    char *fmt;
    fmt = (char*) arg;
    printf(fmt , p->name , p->fname);
}

Cttree *newitem(char *name, char *fname)
{
    Cttree *newp;
    newp = (Cttree *)emalloc(sizeof(Cttree));
    newp->name=name;
    newp->fname=fname;
    /* printf ("new name: %s\n",name); */
    /* printf(" new fname: %s\n",fname); */
    newp->left=NULL;
    newp->right=NULL;
    return newp;
}


Cttree* vcfgetcontacts(FILE *f, int * count);
int   vcfgetncontact(int n);
char * strip(char* tag, char * prefix);
char * unnew(char *ln);
char **cont=NULL;
int maxline = 1;
char buf[200];
Cttree* vcfgetcontacts(FILE *f, int * count)
{
    Cttree * cttree = NULL;
    char *p, **newct;
    int ncon,state,nline;
    cont = (char**) malloc(maxline * sizeof(cont[0]));
    if (cont == NULL){
	return cttree;
    }
    nline= ncon = state = 0;
    while (fgets (buf, sizeof(buf), f)!=NULL) {
	/* remove the \n added by fgets */
	buf[strlen(buf)-1]='\0';
	if (nline >= maxline-1){
	    maxline *=2;
	    newct = (char**) realloc(cont, maxline* sizeof(cont[0]));
	    if (newct == NULL){
		return cttree;
	    }
	    cont = newct;
	}
	if (strncmp(buf,"BEGIN:VCARD",10) == 0){
	    state=1;
	    /*printf ("buf: '%s'\n", buf); */
	} else if(strncmp(buf,"END:VCARD",8)!=0 && state ==1){
	    /* contact line handling  */
	    cont[nline]=strdup(strip(buf,"FN:"));
	    //printf("line: %d - %s\n", nline, cont[nline]);
	    nline++;
	}else if (strncmp(buf,"END:VCARD",8)==0) {
	    if (cont[2]){
		cttree= insert(cttree,newitem(strdup(cont[1]),strdup(cont[2])));
	    }
	    state = 0;
	    nline=0;
	    ncon++;
	    /* printf("\n********************\n"); */
	}
    }
    *count=ncon;
    //p = strdup(buf);
    return cttree;
}


/* strip: strip prefix from tag and remove trailing \n left from fgets */
/* can i manipulate the tag and just return an int if the prefix is found*/
char * strip(char* tag, char * prefix)
{
    if (strncmp(tag,prefix,strlen(prefix)) == 0){
	tag +=strlen(prefix);
    }
    return tag;
}


int main(void)
{
    int nc=0;
    Cttree *cts=NULL;
    /* should have at least a llist or tree as a result of
       vcfgetcontacts so user can access the list for contact info
     */
    if ((cts = vcfgetcontacts(stdin,&nc)) != NULL){
	printf ("ncontacts : '%d'\n", nc);
	applyinorder(cts,printcttree," name:%s \n fname:%s\n");
    }
    return 0;
}
