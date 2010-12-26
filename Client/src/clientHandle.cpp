#include "clientHandle.h"
#include <cstdio>
#include <cstring>
#include <SDL/SDL_mutex.h>

Handle::Handle()
{
	deckEmpty[0] = deckEmpty[1] = false;
	discardEmpty[0] = discardEmpty[1] = false;
	oponentPlaying = myTurn = false;
	for(int i=0;i<6;i++) cards[i] = NULL;

}

Handle::~Handle()
{
	sendLogout();
	sock.close();
}

int Handle::open(const string &hostname){
	sendLogout();
	sock.close();
	return sock.open(hostname);
}

int Handle::sendLogin(const string &user, const string &password)
{
	char *buffer = new char[256];
	buffer[0] = Package::Login;
	sprintf(buffer+1, "%c%s%c%s", user.size(), user.c_str(), password.size(), password.c_str());
	sock.sendData(1+2+user.size()+password.size(), buffer);
	return 0;
}

int Handle::sendLogout()
{	
	char *buffer = new char[1];
	buffer[0] = Package::Logout;
	sock.sendData(1, buffer);
	return 0;
}

int Handle::sendRegister(const string &user, const string &password, const string &mail, const string &name)
{
	char *buffer = new char[256];
	buffer[0] = Package::CreateAccount;
	sprintf(buffer+1, "%c%s%c%s%c%s%c%s",user.size(), user.c_str(), 
										 name.size(), name.c_str(), 
										 password.size(), password.c_str(),
										 mail.size(), mail.c_str());
	sock.sendData(1+4+user.size()+name.size()+password.size()+mail.size(), buffer);	
	return 0;
}

int Handle::sendUseCard(int index){
	char *buffer = new char[2];
	buffer[0] = Package::UseCard;
	buffer[1] = index;
	sock.sendData(2, buffer);
	return 0;
}

int Handle::sendAttack(int index){
	char *buffer = new char[2];
	buffer[0] = Package::Attack;
	buffer[1] = index;
	sock.sendData(2, buffer);
	return 0;
}

int Handle::sendNextTurn(){
	char *buffer = new char[1];
	buffer[0] = Package::NextTurn;
	sock.sendData(1, buffer);
	return 0;
}

Card *Handle::getCard(int idx){
	return cards[idx];
}

int Handle::isDeckEmpty(int player){
	return deckEmpty[player];
}

int Handle::isDiscardEmpty(int player){
	return discardEmpty[player];
}

int Handle::isOponentPlaying(){
	return oponentPlaying;
}

int Handle::isMyTurn(){	
	return myTurn;
}

int Handle::handleIncoming(int &status){
	if(sock.lostConn()){
		status = Package::ConnLost;
		return 0;
	}

	if(!sock.recvEmpty()){
		packet n = sock.recvData();
		if(n.len == 0){
			status = Package::CommError;
			return 0;
		}
		int st = n.data[0];
	
		if(st == Package::CardUpdate){
			if(n.data[2] == 0){
				cards[(int)n.data[1]] = NULL;
			}else{
				cout << "CR(" << (int)n.data[1] << ")=" << (int)(n.data[2]-1) << endl;
				if(n.data[1] < 6){
					if(cards[(int)n.data[1]] != NULL)
						delete cards[(int)n.data[1]];
					cards[(int)n.data[1]] = new Card(n.data[2]-1,0);
					cards[(int)n.data[1]]->energy = n.data[3];
					cards[(int)n.data[1]]->life = n.data[4];
					cards[(int)n.data[1]]->originalLife = n.data[5];
					cards[(int)n.data[1]]->attackAmount = n.data[6];
				}
			}
		}
		if(st == Package::EmptyUpdate){
			deckEmpty[0] = (n.data[1] & 0x01)==0?1:0;
			deckEmpty[1] = (n.data[1] & 0x02)==0?1:0;
			discardEmpty[0] = (n.data[1] & 0x04)==0?1:0;
			discardEmpty[1] = (n.data[1] & 0x08)==0?1:0;
		}		
		
		if(st == Package::YourTurn){
			myTurn = true;
		}
		if(st == Package::OponentTurn){
			myTurn = false;
		}
		
		status = st;
		delete n.data;
		return 1;
	}else{
		return 0;
	}
}
