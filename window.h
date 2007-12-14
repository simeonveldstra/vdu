
/* window.h
 * sim 22 Nov 2007
 */


#include <X11/Xlib.h>
#include <X11/Xutil.h>


//#define DEBUG 

/*
typedef struct _image_node {
  int id;
  XImage * img;
  struct _image_node next;
} image_node;
*/

struct saved_area {
  Bool valid;
  int id;
  int x;
  int y; 
  int dx;
  int dy;
  Pixmap pix;
  struct saved_area * next;
}; 

typedef struct _wind {
  Display * dsp;
  Window w;
  Pixmap pix;
  GC ctx;
  Font fnt;
  Window parent;
  int width, height;
  int min_w, min_h;
  int max_w, max_h;
  int x_off, y_off;
  void (* draw)(struct _wind * w);
  void (* keydown)(struct _wind *w, int x, int y, unsigned int state, unsigned int keycode);
  void (* keyup)(struct _wind *w, int x, int y, unsigned int state, unsigned int keycode);
  void (* buttondown)(struct _wind *w, int x, int y, unsigned int state, unsigned int button);
  void (* buttonup)(struct _wind *w, int x, int y, unsigned int state, unsigned int button);
  void (* motion)(struct _wind * w, int x, int y, unsigned int state);
  char * udata;
  struct saved_area * saved_areas;
  //image_node img_head;
} wind;

typedef struct {
  wind * w;
  XImage * xi;
} image;

typedef struct {
  short x, y;
} point_t;

// prototypes for window.c
wind * mkwind(int width, int height, int min_w, int min_h, int max_w, int max_h, char * name);
void rmwind(wind * w);
void make_pixmap(wind * w);
void copy_to_screen(wind * w);
void event_loop(wind * w);
int getheight(wind * w);
int getwidth(wind * w);
int getdepth(wind * w);
void flush(wind * w);
void color(wind * w, unsigned long c);
void background(wind * w, unsigned long c);
void clear(wind * w, int x, int y, int wth, int hgt);
void point(wind *w, int x, int y);
void line(wind * w, int x1, int y1, int x2, int y2);
void rectangle(wind * w, int x, int y, int wth, int hgt);
void arc(wind * w, int x, int y, int wth, int hgt, int ang1, int ang2);
void fill_rectangle(wind * w, int x, int y, int wth, int hgt);
void fill_polygon(wind * w, point_t * points, int npoints);
void fill_arc(wind * w, int x, int y, int wth, int hgt, int ang1, int ang2);
int text(wind * w, int x, int y, char * str) ;
void setfont(wind * w, char * fontstr);
int save_area(wind * w, int x, int y, int dx, int dy);
Bool saved_area_valid(wind * w, int id);
void invalidate_saved_area(wind * w, int id);
void invalidate_saved_areas(wind * w);
int restore_area(wind * w, int id);
void free_saved_area(wind * w, int id);

