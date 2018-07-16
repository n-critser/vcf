


/*regf: return  1  if pat found or 0 otherwise */
int  regf(const char * txt, const char * pat);


typedef enum {
    S0,
    S1,
    S2
}STATE;

/* regwalk: recusively consume txt return 1 if pat accepts or 0 otherwise */
int regwalk(const char *txt, const char *pat);

