#include "common.h"
#include "graphics.h"
#include "register.h"
#include "alert.h"
#include "md5.h"
#include "card.h"
#include "clienthandle.h"

unsigned int regSelectionIdx = 0;
GLuint regSelectTexture[5] = {0,0,0,0,0};
string regData[4];
GLuint registerTexture=0;
SDL_Surface *registerSurface = NULL;
char passwordMd5[33];
static bool shiftDown = false;

extern Handle *globHandle;
extern Conf *globCn;

int registerSubmit(){
	if(regData[0].size() == 0){
		alertShow("Complete el nombre de usuario", "Error");
		regSelectionIdx = 0;
		return -1;
	}
	if(regData[1].size() == 0){
		alertShow("Complete la contraseña", "Error");
		regSelectionIdx = 1;
		return -1;
	}							
	if(regData[2].size() == 0){
		alertShow("Complete el mail", "Error");
		regSelectionIdx = 2;
		return -1;
	}else{
		if(regData[2].find('@') == string::npos){
			alertShow("Mail incorrecto", "Error");
			regSelectionIdx = 2;
			return -1;
		}
	}									
	if(regData[3].size() == 0){
		alertShow("Complete el nombre", "Error");
		regSelectionIdx = 3;
		return -1;
	}						
	if(globHandle->open(globCn->serverHostName) != 0){
		alertShow("No se puede conectar al servidor", "Error");
		return -2;
	}else{
		md5_buffer_ascii ((const unsigned char *)regData[1].c_str(), regData[1].size(), passwordMd5);
		globHandle->sendRegister(regData[0], passwordMd5, regData[2], regData[3]);
		waitingServerAnswer = true;
		return 0;
	}
}

void registerUpdate(const string &field1, const string &field2, const string &field3, const string &field4, const string &button, const string &title){
	if(regSelectTexture[0] == 0) regSelectTexture[0] = loadImg("res/regsel0.png");
	if(regSelectTexture[1] == 0) regSelectTexture[1] = loadImg("res/regsel1.png");
	if(regSelectTexture[2] == 0) regSelectTexture[2] = loadImg("res/regsel2.png");
	if(regSelectTexture[3] == 0) regSelectTexture[3] = loadImg("res/regsel3.png");
	if(regSelectTexture[4] == 0) regSelectTexture[4] = loadImg("res/regsel4.png");
	
	if(registerSurface == NULL){
		registerSurface = IMG_Load("res/register.png");
	
		SDL_Color white, black;
		white.r = white.g = white.b = 0xFF;
		black.r = black.g = black.b = 0x00;
		
		SDL_Surface *titleText = TTF_RenderUTF8_Blended(font, title.c_str(), white);
		SDL_Rect rTitle = {6,5,0,0};	
		SDL_BlitSurface(titleText, NULL, registerSurface, &rTitle);
		SDL_FreeSurface(titleText);	

		SDL_Surface *userLblText = TTF_RenderUTF8_Blended(font, "Usuario:", black);
		SDL_Rect rUserLbl = {8,48,0,0};
		SDL_BlitSurface(userLblText, NULL, registerSurface, &rUserLbl);
		SDL_FreeSurface(userLblText);		
		
		SDL_Surface *passLblText = TTF_RenderUTF8_Blended(font, "Password:", black);
		SDL_Rect rPassLbl = {8,91,0,0};
		SDL_BlitSurface(passLblText, NULL, registerSurface, &rPassLbl);
		SDL_FreeSurface(passLblText);		
		
		SDL_Surface *mailLblText = TTF_RenderUTF8_Blended(font, "Mail:", black);
		SDL_Rect rMailLbl = {8,134,0,0};
		SDL_BlitSurface(mailLblText, NULL, registerSurface, &rMailLbl);
		SDL_FreeSurface(mailLblText);			
		
		SDL_Surface *nameLblText = TTF_RenderUTF8_Blended(font, "Nombre:", black);
		SDL_Rect rNameLbl = {8,177,0,0};
		SDL_BlitSurface(nameLblText, NULL, registerSurface, &rNameLbl);
		SDL_FreeSurface(nameLblText);					
		
		SDL_Surface *buttonText = TTF_RenderUTF8_Blended(font, button.c_str(), black);
		SDL_Rect rButton = {128,234,0,0};
		rButton.x -= buttonText->w/2;
		rButton.y -= buttonText->h/2;
		SDL_BlitSurface(buttonText, NULL, registerSurface, &rButton);
		SDL_FreeSurface(buttonText);			
		
		registerTexture = genTexture(registerSurface, registerTexture);	
	}
	
	SDL_Color black;
	black.r = black.g = black.b = 0x00;
	
	SDL_Surface *tempSurface = SDL_ConvertSurface(registerSurface, registerSurface->format, registerSurface->flags);
	
	SDL_Surface *f1Text = TTF_RenderUTF8_Blended(font, field1.c_str(), black);
	SDL_Rect rF1 = {88,48,0,0};
	SDL_BlitSurface(f1Text, NULL, tempSurface, &rF1);
	SDL_FreeSurface(f1Text);
	
	SDL_Surface *f2Text = TTF_RenderUTF8_Blended(font, field2.c_str(), black);
	SDL_Rect rF2 = {88,91,0,0};
	SDL_BlitSurface(f2Text, NULL, tempSurface, &rF2);
	SDL_FreeSurface(f2Text);	
	
	SDL_Surface *f3Text = TTF_RenderUTF8_Blended(font, field3.c_str(), black);
	SDL_Rect rF3 = {88,134,0,0};
	SDL_BlitSurface(f3Text, NULL, tempSurface, &rF3);
	SDL_FreeSurface(f3Text);	

	SDL_Surface *f4Text = TTF_RenderUTF8_Blended(font, field4.c_str(), black);
	SDL_Rect rF4 = {88,177,0,0};
	SDL_BlitSurface(f4Text, NULL, tempSurface, &rF4);
	SDL_FreeSurface(f4Text);		
	
	registerTexture = genTexture(tempSurface, registerTexture);	
	SDL_FreeSurface(tempSurface);
}


void registerHandleEvents(SDL_Event *event){
	if(event->type == SDL_KEYDOWN){
		if(event->key.keysym.sym == SDLK_LSHIFT || event->key.keysym.sym == SDLK_RSHIFT){
			shiftDown = true;
		}
		if(event->key.keysym.sym == SDLK_TAB){
			if(shiftDown)
				regSelectionIdx--;
			else
				regSelectionIdx++;
		}
		if(event->key.keysym.sym == SDLK_DOWN){
			if(regSelectionIdx!=4) regSelectionIdx++;
		}
		if(event->key.keysym.sym == SDLK_UP){
			if(regSelectionIdx!=0) regSelectionIdx--;
		}
		regSelectionIdx = (regSelectionIdx + 5) % 5;		
		
		if(regSelectionIdx < 4){
			string temp = regData[regSelectionIdx];

			if(temp.size()<20 && (event->key.keysym.unicode >= (Uint16)' '))
				regData[regSelectionIdx] += (char)event->key.keysym.unicode;
			if((event->key.keysym.sym == SDLK_BACKSPACE)&&(regData[regSelectionIdx].size() != 0 ))
				regData[regSelectionIdx].erase( regData[regSelectionIdx].length() - 1 );
				
			if(regData[regSelectionIdx] != temp){
				string tmp;
				for(size_t i=0;i<regData[1].size();i++)
					tmp += '*';
				registerUpdate(regData[0],tmp,regData[2],regData[3],"Registrarse", "Registro");
			}
		}
	}
	if(event->type == SDL_KEYUP){
		if(event->key.keysym.sym == SDLK_LSHIFT || event->key.keysym.sym == SDLK_RSHIFT){
			shiftDown = false;
		}
		if(event->key.keysym.sym == SDLK_RETURN ||
		   event->key.keysym.sym == SDLK_KP_ENTER){
			registerSubmit();
		}
	}
	if(event->type == SDL_MOUSEBUTTONDOWN){
		int dx = event->button.x - 1024/2+256/2;
		int dy = event->button.y - 768/2+256/2;
		if(dx>83 && dy>43 && dx<244 && dy<67){
			regSelectionIdx = 0;
			
		}
		if(dx>83 && dy>86 && dx<244 && dy<110){
			regSelectionIdx = 1;
			
		}
		if(dx>83 && dy>129 && dx<244 && dy<153){
			regSelectionIdx = 2;
			
		}					
		if(dx>83 && dy>172 && dx<244 && dy<196){
			regSelectionIdx = 3;
			
		}	
		if(dx>93 && dy>223 && dx<163 && dy<244){
			regSelectionIdx = 4;
			
		}						
	}
	if(event->type == SDL_MOUSEBUTTONUP){
		int dx = event->button.x - 1024/2+256/2;
		int dy = event->button.y - 768/2+256/2;
		if(dx>93 && dy>223 && dx<163 && dy<244 && regSelectionIdx == 4){
			registerSubmit();
		}	
	}
}

void registerDraw(){
	drawRect(registerTexture,1024/2-256/2,768/2-256/2,256,256);
	drawRect(regSelectTexture[regSelectionIdx], 1024/2-256/2,768/2-256/2,256,256);
}
