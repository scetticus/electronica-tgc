#ifndef CLIENTHANDLE_H
#define CLIENTHANDLE_H

#include "net.h"
#include "card.h"
#include "../../Server/src/package.h"

class Handle
{
private:
	network sock;
	Card *cards[6];
	bool deckEmpty[2], discardEmpty[2], oponentPlaying, myTurn;
public:
	Handle();
   ~Handle();
	
	int open(const string &hostname);
	
	int sendLogin(const string &user, const string &password);
	int sendRegister(const string &user, const string &password, const string &mail, const string &name);
	int sendLogout();
	
	int sendUseCard(int index);
	int sendAttack(int index);
	int sendNextTurn();
	
	Card *getCard(int idx);	
	int isDeckEmpty(int player);
	int isDiscardEmpty(int player);
	int isOponentPlaying();
	int isMyTurn();
	
	int handleIncoming(int &status);
};

#endif
