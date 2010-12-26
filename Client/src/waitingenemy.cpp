#include "common.h"
#include "graphics.h"
#include "waitingenemy.h"

GLuint loadingTexture = 0;
GLuint waitingTexture=0;

void waitingMsg(char *text, char *title){
	SDL_Surface *waitingSurface = IMG_Load("res/wait.png");

	SDL_Color white, black;
	white.r = white.g = white.b = 0xFF;
	black.r = black.g = black.b = 0x00;
	
	SDL_Surface *titleText = TTF_RenderUTF8_Blended(font, title, white);
	SDL_Rect rTitle = {6,5,0,0};	
	SDL_BlitSurface(titleText, NULL, waitingSurface, &rTitle);
	SDL_FreeSurface(titleText);	

	SDL_Surface *bodyText = TTF_RenderUTF8_Blended(font, text, black);
	SDL_Rect rBody = {8,30,0,0};
	SDL_BlitSurface(bodyText, NULL, waitingSurface, &rBody);
	SDL_FreeSurface(bodyText);		
	
	waitingTexture = genTexture(waitingSurface, waitingTexture);	
	SDL_FreeSurface(waitingSurface);	
}

void waitingEnemyDraw(){
	if(loadingTexture == 0) loadingTexture = loadImg("res/loading.png");
		
	drawRect(waitingTexture, 1024/2-256/2,768/2-128/2,256,128);
	int fIdx = (frame/16)%3+1;
	float st = 0.25 * fIdx;
	drawClippedRect(loadingTexture,1024/2-64/2,768/2-128/2+24+50-5, 64, 16, 0,st, 1,st+0.25);
}

void waitingHandleEvents(SDL_Event *event){
	
}
