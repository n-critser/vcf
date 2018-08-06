#include <stdlib.h>
#include <stdio.h>

typedef struct cttree * Cttree;
typedef struct vcf * Vcf;
struct vcf {
    Cttree tree;
    int count;
};




Vcf  construct();
void destroy(Vcf vcfp);

Cttree insert (Cttree  cttreep, Cttree newctp);
Cttree lookup (Cttree  cttreep, char *name, int search);
Cttree weaksearch(Cttree  cttreep, char*name, int *nct);
void applyinorder(Cttree cttreep,
		  void(*fn)(Cttree, void *), void *arg);

void printcttree(Cttree p, void *arg);
Cttree newitem(const char *name, const char *fname,
	       const char* email, const char * tel);
Cttree vcfgetcontacts(Vcf vcfp,FILE *f, int * count);
int   vcfgetncontact(int n);
char * strip(char* tag, char * prefix, int * found);
char * unnew(char *ln);
