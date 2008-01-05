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
