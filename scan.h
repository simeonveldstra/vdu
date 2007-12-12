
/*
 * scan.h
 *
 * tree node structure
 * sim 9 dec 2007
 */

#define MAXPATH 1024 /* not less than 512 */

typedef struct _DIR {
  char path[MAXPATH];
  struct _DIR * child;  /* eldest child */
  struct _DIR * next;   /* sibling of this node */
  unsigned long long size;
  int nchildren;
  int x, y;
  int dx, dy;
} Ftree;


Ftree * mktree(char * path);
void freetree(Ftree * ft);
