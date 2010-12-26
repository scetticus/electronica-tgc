#ifndef COMMON_H
#define COMMON_H

#define SCREENWIDTH_INTERNAL 1024
#define SCREENHEIGHT_INTERNAL 768
#define NETPORT 8231
#define FPS 60

#include <string>
#include <iostream>
#include <queue>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

extern int gameState;
extern int gameStatePrev;
extern bool waitingServerAnswer;

using namespace std;

typedef struct Point{
	int x, y;
};

namespace gameStates
{
	enum{
		playing=0,
		alert,
		zoom,
		selectionScreen,
		loginScreen,
		waitingEnemy,
		lostConnection,
		registerScreen
	};
}

typedef struct packet{
	size_t len;
	char *data;
};

#endif

