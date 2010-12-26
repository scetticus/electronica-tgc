#include "common.h"
#include "selection.h"
#include "graphics.h"

static unsigned int selectionIdx = 0;
GLuint selectionIdxTexture[2] = {0,0};
GLuint selectionTexture=0;
  
void selectionUpdate(const string &title, const string &first, const string &second){
	if(selectionIdxTexture[0] == 0) selectionIdxTexture[0] = loadImg("res/selection0.png");
	if(selectionIdxTexture[1] == 0) selectionIdxTexture[1] = loadImg("res/selection1.png");
	
	SDL_Surface *selectionSurface = IMG_Load("res/selection.png");

	SDL_Color white, black;
	white.r = white.g = white.b = 0xFF;
	black.r = black.g = black.b = 0x00;
	
	SDL_Surface *titleText = TTF_RenderUTF8_Blended(font, title.c_str(), white);
	SDL_Rect rTitle = {6,5,0,0};	
	SDL_BlitSurface(titleText, NULL, selectionSurface, &rTitle);
	SDL_FreeSurface(titleText);	

	SDL_Surface *firstText = TTF_RenderUTF8_Blended(font, first.c_str(), black);
	SDL_Rect rFirst = {60,67,0,0};
	rFirst.x -= firstText->w/2;
	SDL_BlitSurface(firstText, NULL, selectionSurface, &rFirst);
	SDL_FreeSurface(firstText);		
	
	SDL_Surface *secondText = TTF_RenderUTF8_Blended(font, second.c_str(), black);
	SDL_Rect rSecond = {195,67,0,0};
	rSecond.x -= secondText->w/2;
	SDL_BlitSurface(secondText, NULL, selectionSurface, &rSecond);
	SDL_FreeSurface(secondText);			
	
	selectionTexture = genTexture(selectionSurface, selectionTexture);	
	SDL_FreeSurface(selectionSurface);	
}
 
void selectionHandleEvents(SDL_Event *event){
	if(event->type == SDL_KEYDOWN){
		if(event->key.keysym.sym == SDLK_LEFT || event->key.keysym.sym == SDLK_RIGHT || event->key.keysym.sym == SDLK_TAB)
			selectionIdx = (selectionIdx==0)? 1:0;
	}
	if(event->type == SDL_KEYUP){
		if(event->key.keysym.sym == SDLK_RETURN ||
		   event->key.keysym.sym == SDLK_KP_ENTER){
			if(selectionIdx == 0) 
				gameState = gameStates::loginScreen;
			else
				gameState = gameStates::registerScreen;
		}
	}
	if(event->type == SDL_MOUSEBUTTONDOWN){
		int dx = event->button.x - 1024/2+256/2;
		int dy = event->button.y - 768/2+128/2;

		if(dx>25 && dy>60 && dx<95 && dy<89){
			selectionIdx = 0;
		}
		if(dx>160 && dy>60 && dx<230 && dy<89){
			selectionIdx = 1;
		}
	}
	if(event->type == SDL_MOUSEBUTTONUP){
		int dx = event->button.x - 1024/2+256/2;
		int dy = event->button.y - 768/2+128/2;
		if(dx>25 && dy>60 && dx<95 && dy<89 && selectionIdx == 0){
			gameState = gameStates::loginScreen;
		}
		if(dx>160 && dy>60 && dx<230 && dy<89 && selectionIdx == 1){
			gameState = gameStates::registerScreen;
		}
	}
}

void selectionDraw(){
	drawRect(selectionTexture, 1024/2-256/2, 768/2-128/2, 256, 128);
	drawRect(selectionIdxTexture[selectionIdx], 1024/2-256/2, 768/2-128/2, 256, 128);
}
