#include <stdlib.h>
#include <stdio.h>

typedef struct Cttree  Cttree;

Cttree *insert (Cttree * cttreep, Cttree *newctp);
Cttree *lookup (Cttree * cttreep, char *name, int search);
Cttree *weaksearch(Cttree * cttreep, char*name, int *nct);
void applyinorder(Cttree *cttreep,
		  void(*fn)(Cttree*, void *), void *arg);

void printcttree(Cttree *p, void *arg);
Cttree *newitem(char *name, char *fname, char* email, char * tel);
Cttree* vcfgetcontacts(FILE *f, int * count);
int   vcfgetncontact(int n);
char * strip(char* tag, char * prefix, int * found);
char * unnew(char *ln);
