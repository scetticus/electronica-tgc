#include "logic.h"

Game::Game(Player *p1, Player *p2)
{
	player[0] = p1;
	player[1] = p2;
	turn      = 0;
}

Game::~Game()
{
}

int Game::attack(int attackid)
{	
	if(player[turn] == NULL) return -999;
	
	Card* pl = player[turn]->getBattle();
	if(pl == NULL)
		return -4;
	if(pl->attWait[attackid] > pl->energy)
		return -3; //Chiter
	if(player[(turn+1)%2]->getBattle() == NULL)
		return -2;
		
	pl->energy -= pl->attWait[attackid];
	return player[(turn+1)%2]->attacked(pl->attDmg[attackid]+pl->attackOffset);
	return 0;
}

int Game::nextTurn()
{
	if(player[turn] == NULL) return -999;
	
	if(player[turn]->getBattle() == NULL)
		return -1;
	player[turn]->getBattle()->energy++;
	player[turn]->getBattle()->attackOffset = 0;
	player[turn]->drawCard();
	turn = (turn+1)%2;
	return 0;
}

int Game::selectAttacker(Card* card)
{
	if(player[turn] == NULL) return -999;
	
	if(player[turn]->getBattle() != NULL)
	{
		if(card->life == -1)
			return player[turn]->useObject(card);
		return -1;
	}
	if(card->life == -1) //Intento de usar un item sin batallador activo
		return -2;
		
	player[turn]->setBattle(card);
	return 0;
}
