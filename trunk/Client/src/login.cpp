#include "common.h"
#include "graphics.h"
#include "login.h"
#include "clienthandle.h"
#include "card.h"
#include "md5.h"
#include "alert.h"

extern Handle *globHandle;
extern Conf *globCn;

GLuint loginTexture=0;
SDL_Surface *loginSurface = NULL;
GLuint loginSelectTexture[3] = {0,0,0};
unsigned int loginSelectionIdx = 0;
static bool shiftDown = false;
string loginUsername, loginPassword;

void loginWindowUpdate(const string &username, int pwdLen){
	if(loginSelectTexture[0] == 0) loginSelectTexture[0] = loadImg("res/loginsel0.png");
	if(loginSelectTexture[1] == 0) loginSelectTexture[1] = loadImg("res/loginsel1.png");
	if(loginSelectTexture[2] == 0) loginSelectTexture[2] = loadImg("res/loginsel2.png");
		
	if(loginSurface == NULL){
		loginSurface = IMG_Load("res/login.png");
	
		SDL_Color white, black;
		white.r = white.g = white.b = 0xFF;
		black.r = black.g = black.b = 0x00;
		
		SDL_Surface *titleText = TTF_RenderUTF8_Blended(font, "Login", white);
		SDL_Rect rTitle = {6,5,0,0};	
		SDL_BlitSurface(titleText, NULL, loginSurface, &rTitle);
		SDL_FreeSurface(titleText);	

		SDL_Surface *userLblText = TTF_RenderUTF8_Blended(font, "Usuario:", black);
		SDL_Rect rUserLbl = {8,35,0,0};
		SDL_BlitSurface(userLblText, NULL, loginSurface, &rUserLbl);
		SDL_FreeSurface(userLblText);		
		
		SDL_Surface *passLblText = TTF_RenderUTF8_Blended(font, "Password:", black);
		SDL_Rect rPassLbl = {8,67,0,0};
		SDL_BlitSurface(passLblText, NULL, loginSurface, &rPassLbl);
		SDL_FreeSurface(passLblText);		
		
		SDL_Surface *buttonText = TTF_RenderUTF8_Blended(font, "Aceptar", black);
		SDL_Rect rButton = {128,107,0,0};
		rButton.x -= buttonText->w/2;
		rButton.y -= buttonText->h/2;
		SDL_BlitSurface(buttonText, NULL, loginSurface, &rButton);
		SDL_FreeSurface(buttonText);			
		
		loginTexture = genTexture(loginSurface, loginTexture);	
	}
	
	SDL_Color black;
	black.r = black.g = black.b = 0x00;
	
	SDL_Surface *tempSurface = SDL_ConvertSurface(loginSurface, loginSurface->format, loginSurface->flags);
	
	SDL_Surface *userNameText = TTF_RenderUTF8_Blended(font, username.c_str(), black);
	SDL_Rect rUserName = {91,35,0,0};
	SDL_BlitSurface(userNameText, NULL, tempSurface, &rUserName);
	SDL_FreeSurface(userNameText);
	
	string tempPass;
	for(int i=0;i<pwdLen;i++) tempPass += '*';
	
	SDL_Surface *passWordText = TTF_RenderUTF8_Blended(font, tempPass.c_str(), black);
	SDL_Rect rPassWord = {91,67,0,0};
	SDL_BlitSurface(passWordText, NULL, tempSurface, &rPassWord);
	SDL_FreeSurface(passWordText);	
	
	loginTexture = genTexture(tempSurface, loginTexture);	
	SDL_FreeSurface(tempSurface);
}

void loginDraw(){
	drawRect(loginTexture,1024/2-256/2,768/2-128/2,256,128);
	drawRect(loginSelectTexture[loginSelectionIdx], 1024/2-256/2,768/2-128/2,256,128);
}

void loginHandleEvents(SDL_Event *event){
	if(waitingServerAnswer) return;
	
	if(event->type == SDL_KEYDOWN){
		if(event->key.keysym.sym == SDLK_LSHIFT || event->key.keysym.sym == SDLK_RSHIFT){
			shiftDown = true;
		}
		if(event->key.keysym.sym == SDLK_TAB){
			if(shiftDown)
				loginSelectionIdx --;
			else
				loginSelectionIdx ++;
		}
		if(event->key.keysym.sym == SDLK_DOWN){
			if(loginSelectionIdx!=2) loginSelectionIdx++;
		}
		if(event->key.keysym.sym == SDLK_UP){
			if(loginSelectionIdx!=0) loginSelectionIdx--;
		}
		loginSelectionIdx=(loginSelectionIdx+3) %3;		
		if(loginSelectionIdx == 0){
			string temp = loginUsername;

			if(temp.size()<16){		
				if((event->key.keysym.unicode >= (Uint16)'0')&&( event->key.keysym.unicode <= (Uint16)'9' ))
					loginUsername += (char)event->key.keysym.unicode;
				if((event->key.keysym.unicode >= (Uint16)'a')&&( event->key.keysym.unicode <= (Uint16)'z' ))
					loginUsername += (char)event->key.keysym.unicode;
				if((event->key.keysym.unicode >= (Uint16)'A')&&( event->key.keysym.unicode <= (Uint16)'Z' ))
					loginUsername += (char)event->key.keysym.unicode;
			}
			if((event->key.keysym.sym == SDLK_BACKSPACE)&&(loginUsername.size() != 0 ))
				loginUsername.erase( loginUsername.length() - 1 );
				
			if(loginUsername != temp){
				loginWindowUpdate(loginUsername,loginPassword.size());
			}
		}else if(loginSelectionIdx == 1){
			string temp = loginPassword;

			if(temp.size()<16){		
				if((event->key.keysym.unicode >= (Uint16)' '))
					loginPassword += (char)event->key.keysym.unicode;
			}
			if((event->key.keysym.sym == SDLK_BACKSPACE)&&(loginPassword.size() != 0 ))
				loginPassword.erase( loginPassword.length() - 1 );
				
			if(loginPassword != temp){
				loginWindowUpdate(loginUsername,loginPassword.size());
			}
		}
	}
	if(event->type == SDL_KEYUP){
		if(event->key.keysym.sym == SDLK_LSHIFT || event->key.keysym.sym == SDLK_RSHIFT){
			shiftDown = false;
		}
		if(event->key.keysym.sym == SDLK_RETURN ||
		   event->key.keysym.sym == SDLK_KP_ENTER){
			if(globHandle->open(globCn->serverHostName) != 0){
				alertShow("No se puede conectar al servidor", "Error");
			}else{
				char resbuff[33];
				md5_buffer_ascii ((const unsigned char *)loginPassword.c_str(), loginPassword.size(), resbuff);
				globHandle->sendLogin(loginUsername, resbuff);
				waitingServerAnswer = true;
			}
		}
	}
	if(event->type == SDL_MOUSEBUTTONDOWN){
		int dx = event->button.x - 1024/2+256/2;
		int dy = event->button.y - 768/2+128/2;
		if(dx>86 && dy>30 && dx<247 && dy<54){
			loginSelectionIdx = 0;
			
		}
		if(dx>86 && dy>63 && dx<247 && dy<87){
			loginSelectionIdx = 1;
			
		}
		if(dx>93 && dy>96 && dx<163 && dy<117){
			loginSelectionIdx = 2;
			
		}
	}
	if(event->type == SDL_MOUSEBUTTONUP){
		int dx = event->button.x - 1024/2+256/2;
		int dy = event->button.y - 768/2+128/2;
		if(dx>93 && dy>96 && dx<163 && dy<117 && loginSelectionIdx == 2){
			if(globHandle->open(globCn->serverHostName) != 0){
				alertShow("No se puede conectar al servidor", "Error");
			}else{
				char resbuff[33];
				md5_buffer_ascii ((const unsigned char *)loginPassword.c_str(), loginPassword.size(), resbuff);
				globHandle->sendLogin(loginUsername, resbuff);
				waitingServerAnswer = true;
			}
			
		}
	}
}
