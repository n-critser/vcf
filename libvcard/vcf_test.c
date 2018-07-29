
#include "vcard.h"

int main(void)
{
    int nc=0;
    Cttree *cts=NULL;
    /* should have at least a llist or tree as a result of
       vcfgetcontacts so user can access the list for contact info
     */
    if ((cts = vcfgetcontacts(stdin,&nc)) != NULL){
	printf ("ncontacts : '%d'\n", nc);
	//applyinorder(cts,printcttree,">fname:%s\n name:%s \n email:%s\n tel:%s\n\n");
    }
    //Cttree *lookup (Cttree * cttreep, char *name)
    Cttree *found;
    found = lookup(cts, "Eieland;A",1);
    applyinorder(found,printcttree,
		 "FOUND\n>fname:%s\n name:%s \n email:%s\n tel:%s\n\n");
    int fcount=0;
    found = weaksearch(cts, "Andreas",&fcount);
    applyinorder(found,printcttree,
    		 "FOUND\n>fname:%s\n name:%s \n email:%s\n tel:%s\n\n");
    printf ("found count : %d\n", fcount);
    /* found = lookup(cts, "Eieio"); */
    /* applyinorder(found,printcttree, */
    /* 		 "FOUND\n>fname:%s\n name:%s \n email:%s\n tel:%s\n\n"); */

    return 0;
}
