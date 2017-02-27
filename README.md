
vdu: Visual Disk Usage

vdu is a small native X11 application that creates a window containing
a graphical representation of the directory contents size for a
filesystem directory tree.

The tree is represented as a group of nested boxes of diferent
colors. The size of the box indicates the relative size of that
directory's contents.

When the first mouse button is held down over a box, the path and size
on disk is printed to the top right corner of the window.

It is invoked from the command line as:
$ vdu /path/to/scan/

Scanning large volumes can take some time, as the program stats each
file.

I wrote vdu as an exercise to learn the low level X11 api. It is
written in C, and requires only the X11 and standard C library.



