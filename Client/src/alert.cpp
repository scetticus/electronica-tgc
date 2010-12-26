#include "common.h"
#include "alert.h"
#include "graphics.h"

int gameStatePrev = gameStates::selectionScreen;
GLuint alertTexture=0;
GLuint alertSelectTexture=0;

void alertShow(char *text, char *title){
	if(alertSelectTexture == 0)
		alertSelectTexture = loadImg("res/alertsel0.png");
		
	gameStatePrev = gameState;
	gameState = gameStates::alert;

	SDL_Surface *alertSurface = IMG_Load("res/alert.png");

	SDL_Color white, black;
	white.r = white.g = white.b = 0xFF;
	black.r = black.g = black.b = 0x00;
	
	SDL_Surface *titleText = TTF_RenderUTF8_Blended(font, title, white);
	SDL_Rect rTitle = {6,5,0,0};	
	SDL_BlitSurface(titleText, NULL, alertSurface, &rTitle);
	SDL_FreeSurface(titleText);	

	SDL_Surface *bodyText = TTF_RenderUTF8_Blended(font, text, black);
	SDL_Rect rBody = {8,30,0,0};
	SDL_BlitSurface(bodyText, NULL, alertSurface, &rBody);
	SDL_FreeSurface(bodyText);		
	
	SDL_Surface *buttonText = TTF_RenderUTF8_Blended(font, "Aceptar", black);
	SDL_Rect rButton = {128,106,0,0};
	rButton.x -= buttonText->w/2;
	rButton.y -= buttonText->h/2;
	SDL_BlitSurface(buttonText, NULL, alertSurface, &rButton);
	SDL_FreeSurface(buttonText);			
	
	alertTexture = genTexture(alertSurface, alertTexture);
	SDL_FreeSurface(alertSurface);	
}

void alertHandleEvents(SDL_Event *event){
	if(event->type == SDL_KEYUP){
		if(event->key.keysym.sym == SDLK_RETURN ||
		   event->key.keysym.sym == SDLK_KP_ENTER){
			gameState = gameStatePrev;
		}
	}
	if(event->type == SDL_MOUSEBUTTONDOWN){
		int dx = event->button.x - 1024/2+256/2;
		int dy = event->button.y - 768/2+128/2;
		if(dx > 93 && dy > 96 && 
		   dy < 163 && dy < 117){
			gameState = gameStatePrev;
		}
	}
}

void alertDraw(){
	drawRect(alertTexture, 1024/2-256/2, 768/2-128/2,256,128);
	drawRect(alertSelectTexture, 1024/2-256/2, 768/2-128/2,256,128);
}
