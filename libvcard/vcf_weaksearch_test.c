
#include "vcard.h"
#include <assert.h>
#include <string.h>
int main(int argc, char ** argv)
{

    int i;
    char * target;
    char * fname;
    FILE *file;
    char * usefmt = "USAGE: %s -s <searchterm> -f <file>\n ";
    if (argc > 1){
	for (i = 1 ; i < argc && (argv[i][0] == '-'); i++){
	    /* printf("#%d - '%s' \n",i,argv[i]); */
	    if (strcmp(argv[i],"--help")==0 || strcmp(argv[i],"-h")==0){
		printf("SAW HELP: %s\n",argv[i]);
	    } else if(strcmp(argv[i],"-s")==0 && (i+1 < argc)){
		/* printf("argc = %d \n",argc); */
		/* printf("argv[i+1]= %s\n",argv[i+1]); */
		target = argv[++i];
		printf ("searching for : %s\n",target);
	    } else if(strcmp(argv[i],"-f")==0 && (i+1 < argc)){
		/* printf("argc = %d \n",argc); */
		/* printf("argv[i+1]= %s\n",argv[i+1]); */
		fname = argv[++i];
	    }
	}

    }else {
	fprintf(stderr,usefmt,argv[0]);
	exit(1);
    }

    if((file = fopen(fname,"r"))==NULL){
	fprintf(stderr,"UNABLE TO OPEN FILE %s\n",fname);
	exit(1);
    }


    int nc=0;
    Vcf  v = construct();

    /* should have at least a llist or tree as a result of
       vcfgetcontacts so user can access the list for contact info
     */
    /* printf ("about to get contacts\n"); */
    if ((v->tree = vcfgetcontacts(v,file,&nc)) != NULL){
	printf ("ncontacts : '%d'\n", nc);
    }
    /* fclose(file); */

    assert(v->tree);
    /* applyinorder(v->tree,printcttree, */
    /* 		 "CTS\n>fname:%s\n name:%s \n email:%s\n tel:%s\n\n"); */


    /* assert(cts); */

    /* /\* test weaksearch *\/ */
    /* Vcf  f = construct(); */
    /* /\* Cttree found; *\/ */
    /* int nfnd=0; */
    /* char * sstr=target; */
    /* /\* printf ("searching for : %s\n", sstr); *\/ */
    /* f->tree = weaksearch(v->tree, sstr, &nfnd); */
    /* applyinorder(f->tree,printcttree, */
    /* 		 ">weaksearch fname:%s\n name:%s \n email:%s\n tel:%s"); */
    destroy(v);
    return 0;
}
