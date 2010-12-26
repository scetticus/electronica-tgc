#include "graphics.h"
#include "card.h"
#include "common.h"
#include "md5.h"
#include "clientHandle.h"
#include "alert.h"
#include "selection.h"
#include "waitingenemy.h"
#include "register.h"
#include "login.h"

int gameState = gameStates::selectionScreen;
GLuint zoomTexture=0;
Handle *globHandle;
Conf *globCn;
bool waitingServerAnswer = false;

int main(int argc, char *args[]){
	Conf cn("res/config.gdf");
	globCn = &cn;
	
	graphicsInit(cn.w, cn.h, cn.fs);

	Handle handle;
	globHandle = &handle;
	
	SDL_Event event, event2;
	int quit=0;

	GLuint bgImg = loadImg("res/bg.jpg");
	GLuint lobbyBgImg = loadImg("res/lobbybg.png");
	GLuint cartaBackImg = loadImg("res/cards/cartaback.png");
	GLuint selImg = loadImg("res/cardselection.png");
	GLuint selAtaq[3] = {loadImg("res/attsel1.png"), loadImg("res/attsel2.png"), loadImg("res/attsel3.png")};
	GLuint starImg = loadImg("res/star.png");
	GLuint turnoImg = loadImg("res/turnos.png");
	GLuint hpEmpty = loadImg("res/hp0.png"), hpFull = loadImg("res/hp1.png");
	GLuint spinnerImg = loadImg("res/spinner.png");
	
	waitingMsg("Esperando oponente...", "Información");
	loginWindowUpdate("", 0);
	selectionUpdate("", "Login", "Registro");
	registerUpdate("", "", "", "", "Registrarse", "Registro");
	
	
	int seleccion = 0;
	/****
	Seleccion = 0 => Mano[0]
	Seleccion = 1 => Mano[1]
	Seleccion = 2 => Mano[2]
	Seleccion = 3 => Mano[3]
	
	Seleccion = 4 => Mazo de descarte
	Seleccion = 5 => Carta peleando
	Seleccion = 6 => Mazo
	Seleccion = 7 => Enemigo
	****/
	
	int attIdx = 0;
	/// Lugar donde está el cuadrito selector de ataques
	
	while(!quit){
		graphicsFramePre();
		
		///Manejo de eventos de red
		int status;		
		while(handle.handleIncoming(status)){
			if(status == Package::CardFighting){
				alertShow("No podes mandar a pelear, teniendo a alguien peleando.", "Error");
			}
			if(status == Package::InvalidObjectUse){
				alertShow("No puede usar esa carta sin alguien peleando", "Error");
			}
			if(status == Package::ErrServer){
				alertShow("Error del servidor", "Error");
			}
			if(status == Package::NoTurn){
				alertShow("No es tu turno aun", "Error");
			}
			if(status == Package::NoCard){
				alertShow("No hay carta en esa posicion", "Error");
			}
			if(status == Package::NoEnergy){
				alertShow("No tenes suficiente energia", "Error");
			}
			if(status == Package::NoFighter){
				alertShow("No podes atacar si no tenes bicho", "Error");
			}
			if(status == Package::TurnNoFighter){
				alertShow("No podes pasar de turno sin tener atacante!", "Error");
			}
			if(status == Package::OkAtt){
				handle.sendNextTurn();
			}
			if(status == Package::ErrorLogin){
				alertShow("No se puede loguear", "Error");
			}
			if(status == Package::OkLogin){
				gameState = gameStates::waitingEnemy;
			}
			if(status == Package::ErrorRegister){
				alertShow("Usuario ya existente", "Error");
			}
			if(status == Package::OkRegister){
				alertShow("Cuenta creada correctamente", "Información");
				handle.sendLogin(regData[0], passwordMd5);
			}
			if(status == Package::FoundEnemy){
				gameState = gameStates::playing;
				alertShow("Encontrado Oponente", "Información");
			}
			if(status == Package::OponentLeft){
				gameState = gameStates::waitingEnemy;
				alertShow("El oponente se fue", "Información");
			}
			if(status == Package::ConnLost){
				gameState = gameStates::selectionScreen;
				alertShow("Se perdio la conexion", "Error");
			}
			if(status == Package::CommError){
				alertShow("Error de comunicacion.", "Error");
			}
			waitingServerAnswer = false;
		}
		
		///Manejo de eventos de teclado y mouse		
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT)
				quit = 1;
			
			if(event.type == SDL_KEYDOWN){
				if(event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
			}
			if(event.type == SDL_VIDEORESIZE){
				graphicsResize(event.resize.w, event.resize.h);
			}
			if(gameState == gameStates::playing){
				if(event.type == SDL_KEYUP){
					if(event.key.keysym.sym == SDLK_q){ //atacar/usar carta
						if(seleccion < 4){ //Seleccion en mano
							handle.sendUseCard(seleccion);
							waitingServerAnswer = true;
						}
						if(seleccion == 5){ 
							handle.sendAttack(attIdx);							
							waitingServerAnswer = true;
						}
					}
					if(event.key.keysym.sym == SDLK_e){ //terminar turno
						handle.sendNextTurn();
						waitingServerAnswer = true;
					}
					if(event.key.keysym.sym == SDLK_RETURN){ //zoom
						if(seleccion < 4 || seleccion == 5 || seleccion == 7){
							
							if(seleccion < 4 && handle.getCard(seleccion) != NULL){
								gameState = gameStates::zoom, zoomTexture = handle.getCard(seleccion)->getImg(); continue;
							}
							if(seleccion == 5 && handle.getCard(4) != NULL){
								gameState = gameStates::zoom, zoomTexture = handle.getCard(4)->getImg(); continue;
							}
							if(seleccion == 7 && handle.getCard(5) != NULL){
								gameState = gameStates::zoom, zoomTexture = handle.getCard(5)->getImg(); continue;
							}
						}
					}
				}
				if(event.type == SDL_KEYDOWN){
					if(event.key.keysym.sym == SDLK_LEFT)
						seleccion--;
					
					if(event.key.keysym.sym == SDLK_RIGHT)
						seleccion++;
						
					seleccion = (seleccion+8)%8;
					
					if(seleccion == 5 && handle.getCard(4) != 0){
						if(event.key.keysym.sym == SDLK_UP) attIdx--;
						if(event.key.keysym.sym == SDLK_DOWN) attIdx++;
					}					
				}
			}
			if(gameState == gameStates::zoom){
				if(event.type == SDL_KEYUP)
					if(event.key.keysym.sym == SDLK_RETURN){
						gameState = gameStates::playing;
						continue;
					}
			}			
			event2 = event;
			if(event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP){
				int newX = (int)((int)event2.button.x * ((double)SCREENWIDTH_INTERNAL / (double)globCn->w));
				int newY = (int)((int)event2.button.y * ((double)SCREENHEIGHT_INTERNAL / (double)globCn->h));
				event2.button.x = event2.motion.x = newX;			
				event2.button.y = event2.motion.y = newY;		 					
			}
			if(gameState == gameStates::alert){ alertHandleEvents(&event2); continue; }
			if(gameState == gameStates::selectionScreen){ selectionHandleEvents(&event2); continue; }
			if(gameState == gameStates::waitingEnemy){ waitingHandleEvents(&event2); continue; }
			if(gameState == gameStates::loginScreen){ loginHandleEvents(&event2); continue; }
			if(gameState == gameStates::registerScreen){ registerHandleEvents(&event2); continue; }
		}
		
		///Dibujo
		
		if(gameState == gameStates::waitingEnemy || gameState == gameStates::loginScreen || 
		   gameState == gameStates::alert  || gameState == gameStates::selectionScreen || 
		   gameState == gameStates::registerScreen){
			drawRect(lobbyBgImg, 0,0, 1024,1024);		
		}
		
		if(gameState == gameStates::playing || gameStatePrev == gameStates::playing){	
			drawRect(bgImg, 0,0, 1024,1024);		
			if(seleccion == 7)
				drawRect(selImg, cn.cardCoord[5].x-5,cn.cardCoord[5].y-5, 210,205);
			else
				drawRect(selImg, cn.cardCoord[seleccion+7].x-5,cn.cardCoord[seleccion+7].y-5, 210,205);
				
			for(int i = 0; i<4; ++i){
				drawRect(cartaBackImg, cn.cardCoord[i].x,cn.cardCoord[i].y, 196,196);
				
				if(handle.getCard(i) != NULL)
					drawRect(handle.getCard(i)->getImg(), cn.cardCoord[i+7].x,cn.cardCoord[i+7].y, 196,196);
			}
		
			if(handle.isDeckEmpty(1) == 1)
				drawRect(cartaBackImg, cn.cardCoord[4].x,cn.cardCoord[4].y, 196,196);
			if(handle.isDeckEmpty(0) == 1)
				drawRect(cartaBackImg, cn.cardCoord[13].x,cn.cardCoord[13].y, 196,196);
			
			if(handle.isDiscardEmpty(1) == 1)
				drawRect(cartaBackImg, cn.cardCoord[6].x,cn.cardCoord[6].y, 196,196);
			if(handle.isDiscardEmpty(0) == 1)
				drawRect(cartaBackImg, cn.cardCoord[11].x,cn.cardCoord[11].y, 196,196);
			
			if(handle.getCard(4) != NULL)
				drawRect(handle.getCard(4)->getImg(), cn.cardCoord[12].x,cn.cardCoord[12].y, 196,196);
			if(handle.getCard(5) != NULL)
				drawRect(handle.getCard(5)->getImg(), cn.cardCoord[5].x,cn.cardCoord[5].y, 196,196);
			
			if(attIdx < 0) attIdx = 0;
			if(attIdx > 2) attIdx = 2;
			if(handle.getCard(4) != NULL)
				if(attIdx >= handle.getCard(4)->attackAmount)
					attIdx = handle.getCard(4)->attackAmount-1;
			
			if(seleccion == 5 && handle.getCard(4) != NULL)
				drawRect(selAtaq[attIdx], cn.cardCoord[seleccion+7].x,cn.cardCoord[seleccion+7].y, 196,196);

			if(handle.getCard(4) != NULL){
				//Dibujar estrellas de mi batallador
				for(int i=0;i<handle.getCard(4)->energy;i++){
					drawRect(starImg, cn.cardCoord[12].x+140+32+32*(i/6),cn.cardCoord[12].y+(i%6)*32,32,32);
				}
				//Dibujar salud mia
				drawRect(hpEmpty, cn.cardCoord[12].x+140,cn.cardCoord[12].y,32, 196);
				
				float fracc = 0;		
				int life = handle.getCard(4)->life, orilife = handle.getCard(4)->originalLife;
				if(life < orilife){
					fracc = (float)(orilife - life) / (float)orilife;
				}
				drawClippedRect(hpFull, cn.cardCoord[12].x+140,cn.cardCoord[12].y + fracc * 196 ,32, 196*(1.0-fracc), 0.0, fracc, 1.0, 1.0);
			}
			 
			if(handle.getCard(5) != NULL){
				//Dibujar estrellas del enemigo
				for(int i=0;i<handle.getCard(5)->energy;i++){
					drawRect(starImg, cn.cardCoord[5].x-32-32-32*(i/6),cn.cardCoord[5].y+(i%6)*32,32,32);
				}
				//Dibujar salud del enemigo
				drawRect(hpEmpty, cn.cardCoord[5].x-32,cn.cardCoord[5].y,32, 196);
				
				float fracc = 0;			
				int life = handle.getCard(5)->life, orilife = handle.getCard(5)->originalLife;
				if(life < orilife){
					fracc = (float)(orilife - life) / (float)orilife;
				}
				drawClippedRect(hpFull, cn.cardCoord[5].x-32,cn.cardCoord[5].y + fracc * 196 ,32, 196*(1.0-fracc), 0.0, fracc, 1.0, 1.0);			
			}
			
			if(handle.isMyTurn()){
				drawClippedRect(turnoImg, 0, 0, 256, 128, 0, 0.5, 1, 1);		
			}else{
				drawClippedRect(turnoImg, 0, 0, 256, 128, 0, 0, 1, 0.5);		
			}
			
		}
		if(gameState == gameStates::zoom) drawRect(zoomTexture, 1024/2-364/2, 768/2-512/2,512,512);
		
		if(gameState == gameStates::alert) alertDraw();
		if(gameState == gameStates::loginScreen)loginDraw();
		if(gameState == gameStates::registerScreen) registerDraw();
		if(gameState == gameStates::waitingEnemy) waitingEnemyDraw();
		if(gameState == gameStates::selectionScreen) selectionDraw();
		if(waitingServerAnswer) drawRotatedRect(spinnerImg,1024-70,768-70,64,64,(frame*5)%360);
		
		graphicsFramePost();
	}
	SDL_Quit();
	return 0;
}

