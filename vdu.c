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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>
#include "scan.h"
#include "window.h"
#include "vdu.h"

const unsigned int  colors[] = {0xff0000,
                                0x00ff00,
                                0x0000ff,
                                0x888800,
                                0x880088,
                                0x008888};
const int ncolors = 6;

char * hread(unsigned long long size) {
  char * ret;
  ret = (char *) malloc(sizeof(char) * 32);

  if (size > 1073741824) {
    snprintf(ret, 32, "%4.1fG", (double) size / 1073741824);
  } else if (size > 1048576) {
    snprintf(ret, 32, "%4.1fM", (double) size / 1048576);
  } else if (size > 1024) {
    snprintf(ret, 32, "%4.1fK", (double) size / 1024);
  } else {
    snprintf(ret, 32, "%4lldb", size);
  } 
  ret[31] = '\0';
  return ret;
}

void draw_window(wind * w) {
  void paintdir(Ftree * ft, int x, int y, int dx, int dy, int lvl, int c) {
    Ftree * next;
    double pct;
    int d, dchild, prog;

    c++;
    /* shrink children
    x++; y++; dx -= 2; dy -= 2;
    */
    //* neato 3D stacks
    //x--; y--; dx--; dy--;
    //*/
    ft->x = x;
    ft->y = y;
    ft->dx = dx;
    ft->dy = dy;

    color(w, colors[c % ncolors]);
    fill_rectangle(w, x, y, dx, dy);
    color(w, 0x000000);
    rectangle(w, x, y, dx, dy);

    //slooooooow!
    //copy_to_screen(w);

    if (lvl % 2) {
      d = dx;
      prog = x;
    } else {
      d = dy;
      prog = y;
    }

    /* In the case of more directories than pixels, the rounding of pixel
     * values to the nearest integer creates a cumulative error. This is 
     * not yet a working solution, but it gets it closer than just rounding 
     * to the center of the pixel.
    double roundfactor = 1.0;
    int reqpix = 0;
    next = ft->child;
    while (next) {
      pct = (double) next->size / (double) ft->size;
      reqpix += (int) floor(((double) d * pct) + 0.5);
      next = next->next;
    }
    if (reqpix && d) {
      roundfactor = (double) d / (double) reqpix;
    }
    //printf("lvl: %d, d: %d, rqp: %d, rf:%f\n", lvl, d, reqpix, roundfactor);
     */

    next = ft->child;
    while (next) {
      pct = (double) next->size / (double) ft->size;
      dchild = (int) floor(((double) d * pct) + 0.5);
      //dchild = (int) floor(((double) d * pct * roundfactor)  + 0.4);
      //dchild = (int) floor(((double) d * pct) + (0.5 * roundfactor));
      if (lvl % 2) {
        paintdir(next, prog, y, dchild, dy, lvl + 1, c++);
      } else {
        paintdir(next, x, prog, dx, dchild, lvl + 1, c++);
      }
      prog += dchild;
      next = next->next;
    }
  }
  vdu_data * vd;
  vd = (vdu_data *) w->udata;
  paintdir(vd->ft, 0, 0, getwidth(w), getheight(w), 0, -1);
}

void find_dir(wind * w, int x, int y, unsigned int state, unsigned int code) {
  if (code == 1) {
    char name[MAXPATH] = "";
    unsigned long long size = 0;
    char * str;
    void search(Ftree * ft, int x, int y) {
      Ftree * next;
      if ((x >= ft->x && x <= ft->x + ft->dx) && (y >= ft->y && y <= ft->y + ft->dy)) {
        strcpy(name, ft->path);
        size = ft->size;

        next = ft->child;
        while (next) {
          search(next, x, y);
          next = next->next;
        }
      }
    }
    vdu_data * vd;
    vd = (vdu_data *) w->udata;
    search(vd->ft, x, y);
    str = hread(size);
    printf("%s  %s\n", str, name);

    if ((vd->saved_screen) && (!saved_area_valid(w, vd->saved_screen))) {
      free_saved_area(w, vd->saved_screen);
      vd->saved_screen = 0;
    }
    if (!vd->saved_screen) {
      vd->saved_screen = save_area(w, 0, 0, 0, 0);
    }
    color(w, 0x000000);
    text(w, 3, 21, name);
    text(w, 3, 36, str);
    color(w, 0xFFFFFF);
    text(w, 2, 20, name);
    text(w, 2, 35, str);
    copy_to_screen(w);

    free(str);
  }
}

void destroy_info(wind * w, int x, int y, unsigned int state, unsigned int code) {
  if (code == 1) {
    //printf("\n");
    vdu_data * vd;
    vd = (vdu_data *) w->udata;
    if ((vd->saved_screen) && (saved_area_valid(w, vd->saved_screen))) {
      restore_area(w, vd->saved_screen);
      copy_to_screen(w);
    }
  }
}

int main (int argc, char ** argv) {
  vdu_data * vd;
  int tx, ty;
  char title[256];
  char * path;
  struct stat st;

  if (argc == 2) {
    path = argv[1];
  } else {
    if (!((argc == 1) && (path = getenv("PWD")))) {
      fprintf(stderr, "Visual Disk Usage\ninvocation:\n %s [path]\n", argv[0]);
      exit (1);
    }
  }

  if ((stat(path, &st) == -1) ) { //|| !((st.st_mode & S_IFMT) != S_IFDIR)) {
    fprintf(stderr, "Visual Disk Usage\ninvocation:\n %s [path]\n", argv[0]);
    exit (1);
  }

  snprintf(title, 256, "Visual Disk Usage - %s", path);
  title[255] = '\0';

  wind * w;
  w = mkwind(400,400, 50,50, 0,0, title);

  tx = getwidth(w) / 2;
  tx -= tx * 0.1;
  ty = getheight(w) / 2;
  ty -= ty * 0.05;
  text(w, tx, ty, "Reading...");
  copy_to_screen(w);
  flush(w);

  vd = (vdu_data *) malloc(sizeof(vdu_data));
  if (!vd) {
    fprintf(stderr, "Out of memory\n");
    exit(1);
  }
  vd->ft = mktree(path);
  vd->saved_screen = 0;

  w->udata = (char *) vd;
  w->draw = draw_window;
  w->buttondown = find_dir;
  w->buttonup = destroy_info;

  event_loop(w);

  rmwind(w);
  freetree(vd->ft);
  free(vd);
  return 0;
}

