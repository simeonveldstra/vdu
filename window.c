
/* window.c
*  sim 22 nov 2007
*
*/  


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "window.h"

Atom wm_protocols = 0;
Atom wm_delete_window = 0;
//wm_protocols = wm_delete_window = (Atom) 0;

int checkWind(Window ret) {
  switch (ret) {
    case BadAlloc:
    fprintf(stderr, "BadAlloc\n");
    return ret;
    case BadColor:
    fprintf(stderr, "BadColor\n");
    return ret;
    case BadCursor:
    fprintf(stderr, "BadCursor\n");
    return ret;
    case BadMatch:
    fprintf(stderr, "BadMatch\n");
    return ret;
    case BadPixmap:
    fprintf(stderr, "BadPixMap\n");
    return ret;
    case BadValue:
    fprintf(stderr, "BadValue\n");
    return ret;
    case BadWindow:
    fprintf(stderr, "BadWindow\n");
    return ret;
    default:
    return 0;
  }
}

void _draw(wind * w) {}
void _buttonkey(wind *w, int x, int y, unsigned int state, unsigned int keycode) {}
void _motion(wind *w, int x, int y, unsigned int state) {}

wind * mkwind (int width, int height, int min_w, int min_h, int max_w, int max_h, char * name) {
  wind  * w;
  w = (wind *) malloc(sizeof(wind));
  Window root;
  XTextProperty  * namepty;
  namepty = (XTextProperty *) malloc(sizeof(XTextProperty));
  int screen;
  unsigned long valuemask = 0L;
  unsigned long eventmask = 0L;
  XSetWindowAttributes  attributes;
  unsigned long gcmask = 0L;
  XGCValues gcvalues;

  XSizeHints * hints;
  hints = XAllocSizeHints();
  hints->width = width;
  hints->height = height;
  hints->flags = PSize;
  hints->min_width = min_w;
  hints->min_height = min_h;
  if (min_w && min_h) {
    hints->flags |= PMinSize;
  }
  hints->max_width = max_w;
  hints->max_height = max_h;
  if (max_w && max_h) {
    hints->flags |= PMaxSize;
  }

  w->min_w = min_w;
  w->min_h = min_h;
  w->max_w = max_w;
  w->max_h = max_h;

  XWMHints * wmhints;
  wmhints = XAllocWMHints();
  wmhints->flags = InputHint;
  wmhints->input = True;

  XClassHint * classhint;
  classhint = XAllocClassHint();
  classhint->res_name = (char *) "PYcanvas";
  classhint->res_class = (char *) "PYcanvas";

  w->dsp = XOpenDisplay(NULL);
  if (! w->dsp) {
    fprintf(stderr, "Error opening display.\n");
  }

  XmbTextListToTextProperty(w->dsp, &name, 1, XStringStyle, namepty);

  screen = DefaultScreen(w->dsp);

  valuemask |= (CWBackPixel | CWBorderPixel | CWEventMask);
  attributes.background_pixel = gcvalues.background = WhitePixel(w->dsp,screen); 
  attributes.border_pixel = gcvalues.foreground = BlackPixel(w->dsp,screen);
  eventmask |= (PointerMotionMask 
              | ButtonPressMask 
              | ButtonReleaseMask 
              | KeyPressMask
              | KeyReleaseMask
              | FocusChangeMask
              | ExposureMask 
              | StructureNotifyMask);
  attributes.event_mask = eventmask;

  root = RootWindow(w->dsp, screen);
  w->w = XCreateWindow(w->dsp, root, 0, 0, width, height, 1, 
                                                  0, //CopyFromParent, 
                                                  InputOutput, 
                                                  (Visual *)CopyFromParent, 
                                                  valuemask, 
                                                  &attributes);
  /* Setup for ICCCM delete window. */
  if (! (wm_protocols && wm_delete_window)) {
    wm_protocols = XInternAtom(w->dsp, "WM_PROTOCOLS", False);
    wm_delete_window = XInternAtom(w->dsp, "WM_DELETE_WINDOW", False);
  }
  XSetWMProtocols (w->dsp, w->w, &wm_delete_window, 1);

  XSetWMProperties(w->dsp, w->w, namepty, namepty, (char **) NULL, 0, hints, wmhints, classhint);

  w->fnt = (Pixmap) 0;
  XMapWindow(w->dsp, w->w);

  gcmask |= (GCForeground | GCBackground);
  w->ctx = XCreateGC(w->dsp, w->w, gcmask, &gcvalues);
  w->pix = 0;
  w->draw = _draw;
  w->keydown = _buttonkey;
  w->keyup = _buttonkey;
  w->buttondown = _buttonkey; 
  w->buttonup = _buttonkey;
  w->motion = _motion;
  w->saved_areas = (struct saved_area *) NULL;
  make_pixmap(w);
  XFlush(w->dsp);

#ifdef DEBUG
  fprintf(stderr, "Screen: %d, %dx%d\n", screen, 
                                        DisplayWidth(w->dsp, screen), 
                                        DisplayHeight(w->dsp, screen));
  fprintf(stderr, "Window: 0x%lx, %dx%d\n", w->w, width, height);
#endif
  XFree(hints);
  XFree(wmhints);
  XFree(classhint);
  XFree(namepty);
  return w;
}

void make_pixmap(wind * w) {
  XWindowAttributes getatts;
  XGCValues gval;
  int width, height;
  int oldwidth, oldheight;

  /* limit the new pixmap to mix/max parameter of window */
  XGetWindowAttributes(w->dsp, w->w, &getatts);
  if (w->min_w && w->min_w < getatts.width) {
    if (w->max_w && w->max_w < getatts.width) {
      width = w->max_w;
      w->x_off = -((getatts.width - w->max_w) / 2);
    } else { 
      width = getatts.width;  
    }
  } else {
    width = w->min_w;  
    w->x_off = (getatts.width - w->max_w) / 2;
  }
  if (w->min_h && w->min_h < getatts.height) {
    if (w->max_h && w->max_h < getatts.height) {
      height = w->max_h;
      w->y_off = -((getatts.height - w->max_h) / 2);
    } else {
      height = getatts.height;  
    }
  } else {
    height = w->min_h;  
    w->y_off = (getatts.height - w->max_h) / 2;
  }
  oldwidth = w->width;
  oldheight = w->height;
  w->width = width;
  w->height = height;

  /* don't make a new pixmap if they are both out of bounds */
  if ((oldwidth == w->max_w && oldheight == w->max_h && width == w->max_w && height == w->max_h)
   || (oldwidth == w->min_w && oldheight == w->min_h && width == w->min_w && height == w->min_h)) {
    return;
  }

  if (w->pix) {
    XFreePixmap(w->dsp, w->pix);
  }

  /* invalidate any stored areas */
  invalidate_saved_areas(w);

  w->pix = XCreatePixmap(w->dsp, w->w, width, height, getatts.depth);

  /* Blank new pixmap */
  XGetGCValues(w->dsp, w->ctx, (GCBackground | GCForeground), &gval);
  XSetForeground(w->dsp, w->ctx, gval.background);
  XFillRectangle(w->dsp, w->pix, w->ctx, 0, 0, getatts.width, getatts.height);
  XSetForeground(w->dsp, w->ctx, gval.foreground);

  (*w->draw)(w);
}

void copy_to_screen(wind * w) {
  int sx, sy;
  int dx, dy;
  XWindowAttributes getatts;
  if (w->pix) {
    XGetWindowAttributes(w->dsp, w->w, &getatts);
    /* if window is outside of min/max bounds clip or center pixmap */
    sx = (getatts.width < w->min_w) ? (w->min_w - getatts.width) / 2 : 0;
    sy = (getatts.height < w->min_h) ? (w->min_h - getatts.height) / 2 : 0;
    dx = (getatts.width > w->max_w && w->max_w) ? (getatts.width - w->max_w) / 2 : 0;
    dy = (getatts.height > w->max_h && w->max_h) ? (getatts.height - w->max_h) / 2 : 0;
    XCopyArea(w->dsp, w->pix, w->w, w->ctx, sx, sy, getatts.width, getatts.height, dx, dy);
  }
}

void event_loop(wind * w) {  
  int done = 0;
  Bool resize = False;
  XEvent event;
#ifdef DEBUG
  fprintf(stderr, "Starting Event Loop\n"); 
#endif

  w->draw(w);

  while (!done) {
    XNextEvent(w->dsp, &event);
#ifdef DEBUG
    //printf(":%d  ", event.type);
#endif
    switch(event.type) {
      case ClientMessage: 
      if (event.xclient.message_type == wm_protocols &&
          event.xclient.data.l[0] == wm_delete_window)  {
#ifdef DEBUG
        printf("ICCCM delete window\n");
#endif
        done = 1;
      }
      break;

      case DestroyNotify: 
      done = 1;
      break;

      case ReparentNotify:
      w->parent = w->w;
      Window * childlist;
      unsigned int ujunk;
      Status status;
      while (1) {
        Window rt, par;
        status = XQueryTree(w->dsp, w->parent, &rt, &par, &childlist, &ujunk);
        if (childlist && status) {
          XFree(childlist);
        }
        if (par == rt || !par || !status) break;
        w->parent = par;
      }
      if (w->parent == w->w) {
        w->parent = 0;
#ifdef DEBUG
        fprintf(stderr, "WMFrame equaled window\n");
#endif
      }
#ifdef DEBUG
      fprintf(stderr, "Parent: 0x%lx\n", w->parent);
#endif
      break;

      case ConfigureNotify:
      case MapNotify:
      case Expose:
      while (XCheckMaskEvent(w->dsp, (StructureNotifyMask | ExposureMask), &event)) {
        //printf("%d.", event.type);
        if (event.type == ConfigureNotify) {
          resize = True;
        }
      }
      XWindowAttributes  atts;
      XGetWindowAttributes(w->dsp, w->w, &atts);
      //if (resize & ((w->width != atts.width) | (w->height != atts.height))) {
      if (resize && ((w->width != atts.width) || (w->height != atts.height))) {
        make_pixmap(w);
        resize = False;
      }
      copy_to_screen(w); 
      break;

      case KeyPress:
      w->keydown(w, event.xkey.x, event.xkey.y, event.xkey.state, event.xkey.keycode);
      break;

      case KeyRelease:
      w->keyup(w, event.xkey.x, event.xkey.y, event.xkey.state, event.xkey.keycode);
      break;

      case ButtonPress:
      w->buttondown(w, event.xbutton.x, event.xbutton.y, event.xbutton.state, event.xbutton.button);
      break;

      case ButtonRelease:
      w->buttonup(w, event.xbutton.x, event.xbutton.y, event.xbutton.state, event.xbutton.button);
      break;

      case MotionNotify:
      w->motion(w, event.xmotion.x, event.xmotion.y, event.xmotion.state);
      break;

      default:
      break;
    }
  }
}

void rmwind(wind * w) {
#ifdef DEBUG
  fprintf(stderr, "Destroying Window: 0x%lx\n", w->w);
#endif
  
  //XSync(w->dsp, True);
  //XUnmapWindow(w->dsp, w->w);
  XDestroyWindow(w->dsp, w->w);
  // walk the img list and free them
  if (w->parent) {
#ifdef DEBUG
    fprintf(stderr, "Destroying Window: 0x%lx\n", w->parent);
#endif
    //Might need this:
    //XDestroyWindow(w->dsp, w->parent);
  }
  XFreeGC(w->dsp, w->ctx);
  //XFlush(w->dsp);
  XCloseDisplay(w->dsp);
  free(w);
}

int getheight(wind * w) {
  /*
  XWindowAttributes  atts;
  XGetWindowAttributes(w->dsp, w->w, &atts);
  return atts.height;
  */
  return w->height;
}

int getwidth(wind * w) {
  /*
  XWindowAttributes atts;
  XGetWindowAttributes(w->dsp, w->w, &atts);
  return atts.width;
  */
  return w->width;
}

int getdepth(wind * w) {
  XWindowAttributes atts;
  XGetWindowAttributes(w->dsp, w->w, &atts);
  return atts.depth;
}

/* Drawing Functions */
void flush(wind * w) {
  XFlush(w->dsp);
}

void color(wind * w, unsigned long c) {
  XSetForeground(w->dsp, w->ctx, c);
}

void background(wind * w, unsigned long c) {
  XSetBackground(w->dsp, w->ctx, c);
}

unsigned int get_color(wind * w) {
  XGCValues gval;
  XGetGCValues(w->dsp, w->ctx, GCForeground, &gval);
  return gval.foreground;
}

unsigned int get_background(wind * w) {
  XGCValues gval;
  XGetGCValues(w->dsp, w->ctx, GCBackground, &gval);
  return gval.background;
}

void clear(wind * w, int x, int y, int wth, int hgt) {
  XClearArea(w->dsp, w->pix, x, y, wth, hgt, False);
}

void point(wind *w, int x, int y) {
  XDrawPoint(w->dsp, w->pix, w->ctx, x, y);
  //XDrawPoint(w->dsp, w->w, w->ctx, x, y);
}

void line(wind * w, int x1, int y1, int x2, int y2) {
  XDrawLine(w->dsp, w->pix, w->ctx, x1, y1, x2, y2);
}

void rectangle(wind * w, int x, int y, int wth, int hgt) {
  XDrawRectangle(w->dsp, w->pix, w->ctx, x, y, wth, hgt);
}

void arc(wind * w, int x, int y, int wth, int hgt, int ang1, int ang2) {
  /* angles are in degrees * 64 */
  XDrawArc(w->dsp, w->pix, w->ctx, x, y, wth, hgt, ang1, ang2);
}

void fill_rectangle(wind * w, int x, int y, int wth, int hgt) {
  XFillRectangle(w->dsp, w->pix, w->ctx, x, y, wth, hgt);
}

void fill_polygon(wind * w, point_t * points, int npoints) {
  XFillPolygon(w->dsp, w->pix, w->ctx, (XPoint *) points, npoints, Complex, CoordModeOrigin);
}

void fill_arc(wind * w, int x, int y, int wth, int hgt, int ang1, int ang2) {
  XFillArc(w->dsp, w->pix, w->ctx, x, y, wth, hgt, ang1, ang2);
}

int text(wind * w, int x, int y, char * str) {
  // returns the width of the output text 
  if (! w->fnt) {  // if no font is set, pick one.
    int num;
    Font fnt;
    char ** fnames;
    // Will crash if font is not available 'fixed' is required for X to work.
    fnames = XListFonts(w->dsp, (char *) "*misc*fixed*", 1, &num);
    fnt = XLoadFont(w->dsp, fnames[0]);
    XSetFont(w->dsp, w->ctx, fnt);
    XFreeFontNames(fnames);
    w->fnt = fnt;
  }

  int nchars = 0;
  while (*(str + nchars)) nchars++;

  XDrawString(w->dsp, w->pix, w->ctx, x, y, str, nchars);
  return XTextWidth(XQueryFont(w->dsp, w->fnt), str, nchars);   //return XFontStruct
}

void setfont(wind * w, char * fontstr) {
  Font oldfont = w->fnt;
  Font fnt;
  int sts;

  fnt = XLoadFont(w->dsp, fontstr);
  if (fnt == BadAlloc || fnt == BadName) {
    return; // raise exception
  }
  sts = XSetFont(w->dsp, w->ctx, fnt);
  if (sts == BadAlloc || sts == BadFont) {
    return;
  }
  w->fnt = fnt;
  if (oldfont) {
    XUnloadFont(w->dsp, oldfont);
  }
}

/* saved areas */
int save_area(wind * w, int x, int y, int dx, int dy) {
  struct saved_area * sa;
  struct saved_area * sanext;

  if (0 == dx) {
    dx = getwidth(w);
  }
  if (0 == dy) {
    dy = getheight(w);
  }
  
  sa = (struct saved_area *) malloc(sizeof(struct saved_area));
  if (!sa) {
    fprintf(stderr, "Out of memory...\n");
    exit(1);
  }
  sa->pix = XCreatePixmap(w->dsp, w->w, dx, dy, getdepth(w));
  XCopyArea(w->dsp, w->pix, sa->pix, w->ctx, x, y, dx, dy, 0, 0);

  sa->x = x;
  sa->y = y;
  sa->dx = dx; 
  sa->dy = dy;
  sa->valid = True;
  sa->id = 1;

  sanext = w->saved_areas;
  if (!sanext) {
    w->saved_areas = sa;
    return sa->id;
  }

  while (sanext) {
    if (sa->id == sanext->id) {
      if (sa->id >= INT_MAX) {
        fprintf(stderr, "Pixmap handles are exhausted\n");
        free(sa);
        exit(1);
      }
      sa->id++;
    }
    if (!sanext->next) {
      break;
    }
    sanext = sanext->next;
  }
  sanext->next = sa;
  return sa->id;
}

Bool saved_area_valid(wind * w, int id) {
  struct saved_area * sanext;
  sanext = w->saved_areas;
  while (sanext) {
    if (id == sanext->id) {
      return sanext->valid;
    }
    sanext = sanext->next;
  }
  /*error invalid id */
  return False;
}

void _invalidate_sa(wind * w, int id) {
  struct saved_area * sa;
  sa = w->saved_areas;
  while (sa) {
    if ((id == sa->id) || (id < 0)) {
      sa->valid = False;
      XFreePixmap(w->dsp, sa->pix);
      return;
    }
    sa = sa->next;
  }
}

void invalidate_saved_areas(wind * w) {
  _invalidate_sa(w, -1);
}

void invalidate_saved_area(wind * w, int id) {
  /* is it an error to pass in a negative id? */
  _invalidate_sa(w, id);
}

int restore_area(wind * w, int id) {
  struct saved_area * sa;
  sa = w->saved_areas;
  while (sa) {
    if (id == sa->id) {
      if (!sa->valid) {
        return 0;
      }
      XCopyArea(w->dsp, sa->pix, w->pix, w->ctx, 0, 0, sa->dx, sa->dy, sa->x, sa->y);
      return 1;
    }
    sa = sa->next;
  }
  return -1;
}

void free_saved_area(wind * w, int id) {
  if (!w->saved_areas) {
    fprintf(stderr, "Attempted to free nonexistent pixmap\n");
    exit (1);
  }
  struct saved_area * sanext;
  struct saved_area * sa;
  sa = w->saved_areas;
  sanext = sa->next;
  if (id == sa->id) {
    /* this can't be right...*/
    struct saved_area * tmp;
    tmp = sanext->next;
    w->saved_areas = sanext;
    sanext = sa;
    sanext->next = tmp; 
  }
  do {
    if (id == sanext->id) {
      XFreePixmap(w->dsp, sanext->pix);
      sa->next = sanext->next;
      free(sanext);
    }
    sa = sa->next;
    sanext = sanext->next;
  } while (sanext);
}



