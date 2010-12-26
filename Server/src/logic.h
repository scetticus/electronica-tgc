#ifndef LOGIC_H
#define LOGIC_H

#include "player.h"
#include "card.h"

//Main game class
class Game
{
private:
	Player *player[2];
	int     turn;
public:
	Game(Player *p0, Player *p1);
   ~Game();
    int attack(int attackid);
    int nextTurn();
	int selectAttacker(Card* card);
	int getTurn() {return turn;}
};

#endif
