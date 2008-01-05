/* vdu -- summarize disk usage visually
   Copyright (C) 2007, 2008  Simeon Veldstra

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2. 

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */


/*
 * scan.c
 *
 * scan directories recursively and build a tree with sizes.
 * sim 9 dec 2007
 */

#define _FILE_OFFSET_BITS 64
#include <features.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#include "scan.h"



int rlvl = 0;

int dirfilter(const struct dirent * de) {
  if ((strcmp(de->d_name, ".") == 0) || (strcmp(de->d_name, "..") == 0)) {
    return 0;
  }
  return 1;
}

void addchild(Ftree * parent, Ftree * child) {
  if (!parent || !child) {
    return;
  }
  if (!parent->child) {
    parent->child = child;
  } else {
    Ftree * last = parent->child;
    while (last->next) {
      /* walk backwards through the tree matching inodes to detect loops,
       * this is crap:
      if (last == last->next) {
        fprintf(stderr, "*** loop detected %s || %s ***\n", last->path, last->next->path);
        last->next = (Ftree *) NULL; 
        return;
      }
      if (last == child) {
        fprintf(stderr, "*** Child already in list ***\n");
        return;
      }
      */
      last = last->next;
    }
    last->next = child;
  }
  parent->size += child->size;
  parent->nchildren++;
}

Ftree * mktree(char * path) {
  struct stat st;
  int plen = 0;
  if (lstat(path, &st) == -1) {
    fprintf(stderr, "Can't stat %s.\n", path);
    return (Ftree *) NULL;
  }
  if (!S_ISDIR(st.st_mode)) {
    fprintf(stderr, "%s: not a directory.\n", path);
    return (Ftree *) NULL;
  }

  char fullname[MAXPATH];
  while (path[plen] && plen < (MAXPATH - 257)) {
    fullname[plen] = path[plen];
    plen++;
    if (plen > (MAXPATH - 257)) {
      fprintf(stderr, "MAXPATH overflow\n");
    }
  }
  if (fullname[plen - 1] == '/') {
    plen--;
  }
  if (fullname[plen] != '\0') {
    fullname[plen] = '\0';
  }

  struct dirent ** dlist;
  int nd, i, tplen, npos;
  nd = scandir(path, &dlist, dirfilter, alphasort);
  if (nd == -1)  {
    return (Ftree *) NULL;
  }

  Ftree * ft;
  ft = malloc(sizeof(Ftree));
  if (!ft) {
    if (errno == ENOMEM) {
      fprintf(stderr, "No Memory left!\n");
    }
    fprintf(stderr, "Malloc fails\n");
  }
  ft->child = (Ftree *) NULL;
  ft->next = (Ftree *) NULL;
  ft->size = 0;
  ft->nchildren = 0;
  strcpy(ft->path, fullname);

  if (!dlist) { 
    return ft;
  }

  for (i=0;i<nd;i++) {
    tplen = plen;
    npos = 0;
    fullname[tplen++] = '/';
    while (dlist[i]->d_name[npos]) {
      fullname[tplen++] = dlist[i]->d_name[npos++];
    }
    fullname[tplen] = '\0';
    if (lstat(fullname, &st) == -1) {
      fprintf(stderr, "Can't stat %s %m\n", fullname);
      free(dlist[i]);
      continue;
    }
    if (S_ISDIR(st.st_mode)) {
      ft->size += st.st_size;
      addchild(ft, mktree(fullname));
    } else {
      ft->size += st.st_size;
    }
    free(dlist[i]);
  }
  free(dlist);
  return ft;
}

void freetree(Ftree * ft) {
  Ftree * lst;
  Ftree * it;
  lst = ft->child;
  while (lst) {
    it = lst;
    lst = lst->next;
    freetree(it);
  }
  free(ft);
}



