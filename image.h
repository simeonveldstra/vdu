image * loadImage(wind * w, int width, int height, char * data);
image * newImage(wind * w, int width, int height);
char * readImage(image * img);
image * captureImage(wind * w, int x, int y, int width, int height);
void showImage(image * img, int x, int y);
unsigned int peekImage(image * img, int x, int y);
void pokeImage(image * img, int x, int y, unsigned int color);
void freeImage(image * img);
