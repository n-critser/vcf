/*
  code modeled after that found in openssh=portable
  and Practice Of Programming by Rob Pike and Brian Kernighan
  https://github.com/openssh/openssh-portable/blob/master/xmalloc.c


*/

#include <stdlib.h>

extern	char	*xstrdup(const char *);
extern void     *xmalloc(size_t size);
