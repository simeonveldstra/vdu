
#include <stdio.h>
#include "scan.h"

void pt(Ftree * ft, int parsz, int lvl) {
  float pct;
  int dirsz, i;
  dirsz = ft->size;
  pct = (float) dirsz / (float) parsz * 100;
  
  for (i=0;i<=lvl;i++) {
    printf(" ");
  }
  printf("Node: %s \t\t %2.0f%% (%d/%d)\n", ft->path, pct, dirsz, parsz);
  Ftree * nxt;
  nxt = ft->child;
  while (nxt) {
    pt(nxt, dirsz, lvl + 1);
    nxt = nxt->next;
  }
}

int main (int argc, char ** argv) {
  //printf("Path: %s\n", argv[1]);
  Ftree * ft;
  ft = mktree(argv[1]);

  pt(ft, ft->size, 0);

  freetree(ft);
}

