#include "common.h"
#include "graphics.h"

#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <map>

map <string, int> textures;
static int lastTime = 0;
TTF_Font *font;
unsigned int frame = 0;

int graphicsInit(int _w, int _h, int _fs){
	putenv("SDL_VIDEO_WINDOW_POS=center");
	putenv("SDL_VIDEO_CENTERED=1");
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	int fs = _fs?SDL_FULLSCREEN:0;
	SDL_SetVideoMode(_w,_h,32, SDL_OPENGL | SDL_HWSURFACE | fs);
	SDL_WM_SetCaption("",NULL);
	glViewport(0,0,_w,_h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,SCREENWIDTH_INTERNAL,SCREENHEIGHT_INTERNAL,0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor4d( 1.0,1.0,1.0,1.0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.9f, 0.9f, 0.9f, 0.1f);
	SDL_EnableUNICODE( SDL_ENABLE );
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	font = loadFont("res/trebuc.ttf", 12);
	return 0;
}

int drawRect(GLuint img, int x, int y, int w, int h){
	glBindTexture(GL_TEXTURE_2D, img);
	
	glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3f( x  ,   y, 0);
		glTexCoord2d(1, 0); glVertex3f( x+w,   y, 0);
		glTexCoord2d(1, 1); glVertex3f( x+w, y+h, 0);
		glTexCoord2d(0, 1); glVertex3f( x  , y+h, 0);
	glEnd();

	return 0;
}

int drawRotatedRect(GLuint img, int x, int y, int w, int h, int rot){
	glBindTexture(GL_TEXTURE_2D, img);
	
	glPushMatrix();
	glTranslatef(x+w/2,y+h/2,0);	
	glRotatef(rot,0,0,1);		
	
	glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3f( -w/2, -h/2, 0);
		glTexCoord2d(1, 0); glVertex3f( +w/2, -h/2, 0);
		glTexCoord2d(1, 1); glVertex3f( +w/2, +h/2, 0);
		glTexCoord2d(0, 1); glVertex3f( -w/2, +h/2, 0);
	glEnd();
	
	glPopMatrix();
	
	return 0;
}

int drawClippedRect(GLuint img, float x, float y, float w, float h, float ws, float hs, float wc, float hc){
	glBindTexture(GL_TEXTURE_2D, img);
	
	glBegin(GL_QUADS);
		glTexCoord2d(ws, hs); glVertex3f( x  ,   y, 0);
		glTexCoord2d(wc, hs); glVertex3f( x+w,   y, 0);
		glTexCoord2d(wc, hc); glVertex3f( x+w, y+h, 0);
		glTexCoord2d(ws, hc); glVertex3f( x  , y+h, 0);
	glEnd();
	
	return 0;
}

GLuint loadImg(const string &filename){
	//cout << "Cargando " << filename << " a los " << SDL_GetTicks() << "ms." << endl;
	
	if(textures.count(filename) > 0){
		//cout << "Precargada" << endl;
		return textures[filename];
	}
	
	GLuint imgId;
	SDL_Surface *loadedImage = IMG_Load(filename.c_str()); 
	
	if(loadedImage == NULL) return 0;
	
	SDL_Surface *optImage = SDL_DisplayFormatAlpha(loadedImage);
	
	if(optImage == NULL) return 0;
	
	glGenTextures(1, &imgId);
	glBindTexture(GL_TEXTURE_2D, imgId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);	
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, loadedImage->w,loadedImage->h, 0x80E1, GL_UNSIGNED_BYTE, optImage->pixels);
	SDL_FreeSurface( loadedImage );
	SDL_FreeSurface( optImage );
	
	textures[filename] = imgId;
	
	//cout << "Fin carga a los " << SDL_GetTicks() << "ms." << endl;
	return imgId;
}

int graphicsFramePre(bool clear){
	if(clear)
		glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	lastTime = SDL_GetTicks();
	return 0;
}

int graphicsFramePost(){
	SDL_GL_SwapBuffers();
	
	int diff = SDL_GetTicks() - lastTime; 	
	int desiredMs = 1000/FPS;
	if(diff < desiredMs)
		SDL_Delay(desiredMs - diff);
		
	frame++;
	return 0;
}

int graphicsResize(int w, int h){
	glViewport(0,0,w,h);
	return 0;
}

TTF_Font* loadFont(const string &filename, int size){
	TTF_Font* font = TTF_OpenFont(filename.c_str(), size);
	
	if(font == NULL)
		cerr << "Error cargando fuente " << filename << ": " << TTF_GetError() << endl;
	return font;
}

void setColor(float r, float g, float b, float a){
	glColor4f(r,g,b,a);
}

GLuint genTexture(SDL_Surface *srf, GLuint tex){
	if(tex == 0)
		glGenTextures(1, &tex);
		
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, srf->w, srf->h, 0,GL_RGBA, GL_UNSIGNED_BYTE, srf->pixels);
	
	return tex;
}
