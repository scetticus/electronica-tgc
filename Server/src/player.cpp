#include "player.h"

Player::Player(User *user){
	//Player:
	/*
	deque<card*> deck, discard;
	card*        hand[4];
	card*        battle;
	*/

	//User:
	/*
	string name, passw;
	int deck[40]; //40 nomas
	vector<int> allCards; //todas las cartas q tiene
	*/
	this->user = user;
	deck.clear(); discard.clear();
	hand[0] = hand[1] = hand[2] = hand[3] = NULL;
	battle  = NULL;
	life    = 5; //por default, ver
}

Player::~Player(){
	for(deque<Card*>::iterator it = deck.begin(); it != deck.end(); ++it) if(*it)
		delete *it;
	for(deque<Card*>::iterator it = discard.begin(); it != discard.end(); ++it) if(*it)
		delete *it;
	for(int i = 0; i<4; ++i) if(hand[i])
		delete hand[i];
		
	if(battle) delete battle;
	if(user)   delete user;
}

void Player::shuffleDeck(){

	vector<int> d(40);
	
	for(int i = 0; i<40; ++i) d[i] = user->deck[i];
	for(int i = 0; !d.empty(); ++i)
	{
		int pos = rand()%d.size();
		deck.push_back(new Card(d[pos], i));
		d.erase(d.begin()+pos);
	}
	
	for(int i = 0; i<4; ++i) hand[i] = deck.front(), deck.pop_front();
}

Card *Player::getHand(int i){
	//cerr << "=O ME TAN VIENDO LA MANO\n" << endl;
	return hand[i];
}
Card *Player::getBattle(){
	return battle;
}
bool Player::isDeckEmpty(){
	return deck.size()==0;
}
bool Player::isDiscardEmpty(){
	return discard.size()==0;
}

void Player::setBattle(Card* card)
{
	if(battle != NULL)
	{
		cerr << "Player::setBattle(NULL);" << endl;
		return;
	}

	battle = card;
	removeCard(card, false);
}

void Player::removeCard(Card* card, bool discard)
{
	for(int i = 0; i<4; ++i) if(hand[i] != NULL && hand[i]->id == card->id)
		hand[i] = NULL;
	
	if(discard)	this->discard.push_back(card);
}

int Player::attacked(int dmg)
{
	battle->life -= dmg;
	
	if(battle->life <= 0){
		removeCard(battle);
		delete battle, battle = NULL;
		return --life, 0;
	}
	return -1;
}


/*
namespace objActions
{
	enum actions{
		viewOpCards=0, 
		incAttack,
		heal,
		incEnergy
	};
};*/

int Player::useObject(Card* card)
{
	switch(card->action)
	{
		case objActions::viewOpCards:
			break;
		case objActions::incAttack:
			if(battle == NULL)
				return -2;
			battle->attackOffset += card->amount;
			break;
		case objActions::heal:
			if(battle == NULL)
				return -2;
			battle->life += card->amount;
			break;
		case objActions::incEnergy:
			if(battle == NULL)
				return -2;
			battle->energy += card->amount;
			break;
		default:
			cout << "ACCION NO RECONOCIDA" << endl;
			return -3;
	}
	
	removeCard(card);
	
	return 0;
}

void Player::drawCard()
{
	if(deck.empty()) return;
	
	for(int i = 0; i<4; ++i) if(hand[i] == NULL){
		hand[i] = deck.front();
		deck.pop_front();
		break;
	}
}
