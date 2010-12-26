#ifndef SERVERHANDLE_H
#define SERVERHANDLE_H

#include "common.h"
#include "package.h"
#include "net.h"
#include "player.h"
#include "logic.h"
#include <queue>

struct packet{
	size_t len;
	char *data;
};

class Handle
{
private:
	u_int sd;
	Player *player;
	Handle *enemy;
	Game   *game;
	queue <packet>sendQueue;
	SDL_sem *sendSem;
	SDL_Thread *sendThread;
	static int sendThreadHelper(void *_t);
	int sendThreadFn();
	void waitFindOponent();
	int netStatus, status;
	int turnid;
	bool logged;
public:
	Handle(u_int s);
	~Handle();
	int handle();
	int quit();
	void bgSendData(size_t len, char *data);
	void setGame(Game *g);
	int startBattle(Handle *ene);
	void sendCards();	
	Player *getPlayer(){
		return player;
	}
	void endMatch();
};



#endif
