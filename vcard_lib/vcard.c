#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "vcard.h"
#include "../../eprintf.h"

int hamming_distance(unsigned x, unsigned y);
int hamstring(char * newname, char * treename);
struct Cttree{
    char *name;
    char *fname;
    char *tel;
    char *email;
    char *addr;
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


Cttree *lookup (Cttree * cttreep, char *name, int search)
{
    int cmp ;
    if (cttreep == NULL){
	return NULL;
    }
    cmp = strcmp(name, cttreep->name);
    int hamdist=hamstring(name,cttreep->name);
    char * treefound=strstr(name,cttreep->name);
    char * namefound=strstr(cttreep->name,name);
    if (cmp == 0){
	return cttreep;
    } else if( search && (hamdist < 20 || treefound !=NULL
			  ||namefound !=NULL)){
	size_t reject = strcspn(name, cttreep->name);
	printf ("reject : %d\n ", reject);
	size_t accept = strspn(name, cttreep->name);
	printf ("accept : %d\n ", accept);
	printf ("FOUND CLOSE MATCH\n");
	printf ("hamstring : %d - name: %s - treename: %s\n",
		hamdist, name, cttreep->name);
	return cttreep;
    } else if( cmp < 0){
	return lookup(cttreep->left, name,search);
    } else {
	return lookup(cttreep->right, name,search);
    }
}

int hamstring(char * newname, char * treename)
{
    int newlen,treelen,counter,i,hamd;
    hamd=0;
    newlen=strlen(newname);
    treelen=strlen(treename);
    /* set counter to smallest len*/
    counter=newlen > treelen ? treelen : newlen;
    for (i = 0 ; i < counter+1; i++){
	hamd+=hamming_distance(newname[i],treename[i]);
    }
    /* add the distance between string lengths to overall hammind distance */
    hamd+= (newlen - treelen)>=0? (newlen - treelen): (treelen - newlen);
    return hamd;
}
/* binary string hamming distance https://en.wikipedia.org/wiki/Hamming_distance */
int hamming_distance(unsigned x, unsigned y)
{
    int dist = 0;
    unsigned  val = x ^ y;

    // Count the number of bits set
    while (val != 0)
    {
        // A bit is set, so increment the count and clear the bit
        dist++;
        val &= val - 1;
    }

    // Return the number of differing bits
    return dist;
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
    printf(fmt , p->fname, p->name , p->email,p->tel);
}

Cttree *newitem(char *name, char *fname, char* email, char * tel)
{
    Cttree *newp;
    newp = (Cttree *)emalloc(sizeof(Cttree));
    newp->name=name;
    newp->fname=fname;
    newp->email=email;
    newp->tel = tel;
    /* printf ("new name: %s\n",name); */
    /* printf(" new fname: %s\n",fname); */
    newp->left=NULL;
    newp->right=NULL;
    return newp;
}

char **cont=NULL;
int maxline = 1;
char buf[200];
Cttree* vcfgetcontacts(FILE *f, int * count)
{
    Cttree * cttree = NULL;
    char *p, **newct;
    int ncon,state,nline,found;
    cont = (char**) malloc(maxline * sizeof(cont[0]));
    if (cont == NULL){
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
	    if ((cont[nline]=strdup(strip(buf,"FN:", &found))) && found){
		fnline=nline;
		/* printf ("fnline:%d\n  - fn:%s\n",fnline,cont[fnline]); */
	    } else if((cont[nline]=strdup(strip(buf,
						"N:", &found))) && found){
		nameline=nline;
	    }else if((cont[nline]=strdup(strip(buf,
					       "EMAIL", &found))) && found){
		emailline=nline;
	    }else if((cont[nline]=strdup(strip(buf,
					       "TEL;", &found))) && found){
		telline=nline;
	    }else {
		cont[nline] = strdup(buf);
		/* printf("unparsed line : %s\n",buf); */
	    }
	    //printf ("fnline:%d  - fn:'%s'",fnline,cont[fnline]);
	    //printf ("found: %d\n",found);
	    //printf("line: %d - %s\n", nline, cont[nline]);
	    nline++;
	}else if (strncmp(buf,"END:VCARD",8)==0) {
	    if (cont[fnline]){
		cttree= insert(cttree,newitem(strdup(cont[nameline]),strdup(cont[fnline]), strdup(cont[emailline]), strdup(cont[telline])));
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
    *count=ncon;
    //p = strdup(buf);
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
	for(i=strlen(tag) ; i >=0&& !isalpha(tag[i]); i-- ){
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

