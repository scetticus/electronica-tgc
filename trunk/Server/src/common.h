#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
using namespace std;

#define GAME_PORT 8231

extern SDL_sem *waitListSemaphore;

namespace HandleStatus{
	enum Status{
		Nothing=0,
		Waiting,
		Fighting
	};
}

#endif
