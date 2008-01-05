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


/* pycanvas image.c
 *
 * routines to manipulate pixmaps
 * sim - 24 Nov 2007
 */

#include <stdlib.h>
#include "window.h"


image * loadImage(wind * w, int width, int height, char * data) {
  XImage * xi;
  image * img;
  img = (image *) malloc(sizeof(image));
  xi = XCreateImage(w->dsp, 
                    XDefaultVisualOfScreen(DefaultScreenOfDisplay(w->dsp)),
                    getdepth(w),
                    ZPixmap,
                    0, //offset
                    data, // is not copied by xlib
                    width,
                    height,
                    32, //bitmap pad
                    0); //bytes per line
  img->xi = xi;
  img->w = w;
  return img;
}

image * newImage(wind * w, int width, int height) {
  image * img;
  char * data;
  data = (char *) malloc(width * height * 4); 
  img = loadImage(w, width, height, data);
  //free(data);
  return img;
}

char * readImage(image * img) {
  char * data;
  int size, i;
  size = img->xi->width * img->xi->height;  //32 bit pixels
  data = (char *) malloc(size * sizeof(char) * 4);
  for (i = 0; i <= size * 4; i++) {
    data[i] = img->xi->data[i];
  }
  return data;
}

// Check for failure here!!
image * captureImage(wind * w, int x, int y, int width, int height) {
  image * img;
  img = (image *) malloc(sizeof(image));
  img->w = w;
  img->xi = XGetImage(w->dsp, w->pix, x, y, width, height, (unsigned long) 0x00FFFFFF, ZPixmap);
  return img;
}
  
void showImage(image * img, int x, int y) {
  XPutImage(img->w->dsp, img->w->pix, img->w->ctx, img->xi, 0, 0, x, y, img->xi->width, img->xi->height);
}

unsigned int peekImage(image * img, int x, int y) {
  return XGetPixel(img->xi, x, y);
}

void pokeImage(image * img, int x, int y, unsigned int color) {
  XPutPixel(img->xi, x, y, color);
}

void freeImage(image * img) {
  XDestroyImage(img->xi);
  free(img);
}

/*
image * loadImage(wind * w, int width, int height, char * data) {
  XImage * xi;
  image * img;
  img = (image *) malloc(sizeof(image));
  xi = XCreateImage(w->dsp, 
                    XDefaultVisualOfScreen(DefaultScreenOfDisplay(w->dsp)),
                    getdepth(w),
                    ZPixmap,
                    0, //offset
                    data, // is copied by xlib
                    width,
                    height,
                    32, //bitmap pad
                    0); //bytes per line
  img->xi = xi;
  img->w = w;
  return img;
}


image * newImage(wind * w, int width, int height) {
  image * img;
  char * data;
  data = (char *) malloc(width * height * 4); 
  img = loadImage(w, width, height, data);
  free(data);
  return img;
}

char * readImage(image * img) {
  return img->xi->data;
}

image * captureImage(wind * w, int x, int y, int width, int height) {
  image * img;
  img = (image *) malloc(sizeof(image));
  img->w = w;
  img->xi = XGetImage(w->dsp, w->w, x, y, width, height, (unsigned long) 0x00FFFFFF, ZPixmap);
  return img;
}
  
void showImage(image * img, int x, int y) {
  XPutImage(img->w->dsp, img->w->w, img->w->ctx, img->xi, 0, 0, x, y, img->xi->width, img->xi->height);
}

unsigned int peekImage(image * img, int x, int y) {
  return XGetPixel(img->xi, x, y);
}

void pokeImage(image * img, int x, int y, unsigned int color) {
  XPutPixel(img->xi, x, y, color);
}

void freeImage(image * img) {
  XDestroyImage(img->xi);
}


 */
