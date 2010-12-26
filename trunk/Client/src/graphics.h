#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "common.h"

extern unsigned int frame;
extern TTF_Font *font;
int graphicsInit(int w, int h, int _fs);
int drawRect(GLuint img, int x, int y, int w, int h);
int drawRotatedRect(GLuint img, int x, int y, int w, int h, int rot);
int drawClippedRect(GLuint img, float x, float y, float w, float h, float ws, float hs, float wc, float hc);
GLuint loadImg(const string &filename);
int graphicsFramePre(bool clear=false);
int graphicsFramePost();
int graphicsResize(int w, int h);
void setColor(float r, float g, float b, float a);
TTF_Font* loadFont(const string &filename, int size);
GLuint genTexture(SDL_Surface *srf, GLuint tex=0);

#endif
