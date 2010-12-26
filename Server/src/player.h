#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"
#include "package.h"
#include <queue>
using namespace std;

class Player
{
private:
	deque<Card*> deck, discard;
	Card*        hand[4];
	Card*        battle;
	int			 life;
public:
	Player(User* user);
   ~Player();
	void shuffleDeck();
	void setBattle(Card* card);
	int attacked(int dmg);
	Card *getHand(int i);
	Card *getBattle();
	bool isDeckEmpty();
	bool isDiscardEmpty();
	int  useObject(Card* card);
	void drawCard();
	
	User* user;
private:
	void removeCard(Card* card, bool discard=true);
};

#endif
