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

image * loadImage(wind * w, int width, int height, char * data);
image * newImage(wind * w, int width, int height);
char * readImage(image * img);
image * captureImage(wind * w, int x, int y, int width, int height);
void showImage(image * img, int x, int y);
unsigned int peekImage(image * img, int x, int y);
void pokeImage(image * img, int x, int y, unsigned int color);
void freeImage(image * img);
