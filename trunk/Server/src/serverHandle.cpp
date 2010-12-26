#include <fstream>
#include "serverHandle.h"
#include "common.h"

list<Handle*> waitList;
SDL_sem *waitListSemaphore = NULL;

int Handle::sendThreadFn(){
	while(1){
		SDL_SemWait(sendSem);
		if(!sendQueue.empty()){
			packet n = sendQueue.front();
			sendQueue.pop();
			SDL_SemPost(sendSem);
			if(sendData(sd, sizeof(size_t), (const char*) &n.len) < 0) netStatus = 0;
			if(sendData(sd, n.len, n.data) < 0) netStatus = 0;
			delete n.data;
		}else{
			SDL_SemPost(sendSem);
		}
		SDL_Delay(1);
	}
	return 0;
}

int Handle::sendThreadHelper(void *_t){
	Handle *t = (Handle*) _t;
	return t->sendThreadFn();
}

Handle::Handle(u_int _sd)
{
	if(waitListSemaphore == NULL) waitListSemaphore = SDL_CreateSemaphore(1);
	sd = _sd;
	player = NULL;
	sendSem = SDL_CreateSemaphore( 1 );
	sendThread = SDL_CreateThread(sendThreadHelper, this);
	netStatus = 1;
	status = HandleStatus::Nothing;
	enemy = NULL;  
	game = NULL;
	turnid = 0;
	srand(time(NULL));
	logged = false;
}

Handle::~Handle()
{
	SDL_KillThread(sendThread);
	SDL_DestroySemaphore(sendSem);
}

int Handle::handle()
{
	packet n;
	if(recvData(sd, sizeof(size_t), (char*)&n.len) < 0) return 1;
	if(n.len <= 0) return 1;
	n.data = new char[n.len];
	if(recvData(sd, n.len, n.data) < 0){
		delete n.data;
		return 1;
	}

	if(n.data[0] == Package::Login){
		if(logged==false && player == NULL){
			int sizeUsername = n.data[1];
			int sizePassword = n.data[2+sizeUsername];
			
			char username[32];
			memcpy(username, n.data+2, sizeUsername);
			username[sizeUsername] = 0;
		
			char password[64];
			memcpy(password, n.data+2+sizeUsername+1, sizePassword);
			password[sizePassword] = 0;
			
			string filename;
			filename = "data/";
			filename += username;
			filename += ".udf";
					
			player = new Player(new User(filename));

			char *buffer = new char[1];	
			
			if(string(password).substr(0,32) == player->user->passw.substr(0,32)){
				buffer[0] = Package::OkLogin;
				cout << "Usuario " << username << " se logueo" << endl;		
				logged = true;
			}else{
				buffer[0] = Package::ErrorLogin;
				cout << "* Error login: usuario " << username << endl;
				
				cout << string(password) << " VS " << player->user->passw << endl;
				
				delete player;
				player = NULL;
				logged = false;
			}
			bgSendData(1, buffer);
			if(logged){
				waitFindOponent();
			}
		}
	}
	if(n.data[0] == Package::Logout){
		if(player){
			cout << "Package::Logout de " << player->user->name << endl;
			char *buffer = new char[1];	
			buffer[0] = Package::OkLogout;
			bgSendData(1, buffer);
			
			delete player;
			player = NULL;
			logged = false;
		}
	}
	if(n.data[0] == Package::UseCard){
		if(player && game && enemy){
			char *buffer = new char[1];
			
			if(game != NULL){
				if(game->getTurn() != turnid)
					buffer[0] = Package::NoTurn;
				else if(player->getHand(n.data[1]) == NULL)
					buffer[0] = Package::NoCard;
				else{
					switch(game->selectAttacker(player->getHand(n.data[1]))){
						case -3:
							buffer[0] = Package::ErrServer;
							break;
						case -2:
							buffer[0] = Package::InvalidObjectUse;
							break;
						case -1:
							buffer[0] = Package::CardFighting;
							break;
					}
				}
			}
			bgSendData(1, buffer);
			
			sendCards();
			enemy->sendCards();
		}
	}
	if(n.data[0] == Package::Attack){
		if(player && game && enemy){
			char *buffer = new char[1];
			if(game != NULL){
				if(game->getTurn() != turnid)
					buffer[0] = Package::NoTurn;
				else{
					switch(game->attack(n.data[1])){
						case -4:
							buffer[0] = Package::NoCard;
							break;
						case -3:
							buffer[0] = Package::NoEnergy;
							break;
						case -2:
							buffer[0] = Package::NoFighter;
							break;
						case -1:
						case 0:
							buffer[0] = Package::OkAtt;
							break;
						default:
							buffer[0] = Package::ErrServer;
					}
				}
			}
			bgSendData(1, buffer);			
			sendCards();
			enemy->sendCards();	
		}
	}	
	if(n.data[0] == Package::NextTurn){
		if(player && game && enemy){
			char *buffer = new char[1];	
			if(game->getTurn() == turnid){
				if(game->nextTurn() == -1)
					buffer[0] = Package::TurnNoFighter;
				else
					buffer[0] = Package::Nothing;
			}else{
				buffer[0] = Package::NoTurn;
			}	
			bgSendData(1, buffer);
			sendCards();
			enemy->sendCards();
		}
	}
	if(n.data[0] == Package::CreateAccount){
		if(!logged && !player){
			int sizeUsername = 	n.data[1];
			int sizeName = 		n.data[1+1+sizeUsername];
			int sizePassword = 	n.data[1+1+sizeUsername+1+sizeName];
			int sizeMail = 		n.data[1+1+sizeUsername+1+sizeName+1+sizePassword];
			char _username[256], _name[256], _password[256], _mail[256];
			memset(_username, 0, 256);
			memset(_name, 0, 256);
			memset(_password, 0, 256);
			memset(_mail, 0, 256);
			strncpy(_username, 	n.data+1+1, sizeUsername);
			strncpy(_name, 		n.data+1+1+sizeUsername+1, sizeName);
			strncpy(_password, 	n.data+1+1+sizeUsername+1+sizeName+1, sizePassword);
			strncpy(_mail, 		n.data+1+1+sizeUsername+1+sizeName+1+sizePassword+1, sizeMail);
			
			string filename;
			filename = "data/";
			filename += _username;
			filename += ".udf";
			
			ifstream ifile;
			ofstream ofile;
			ifile.open(filename.c_str());
			
			if(ifile.is_open()){
				char *buffer = new char[1];	
				buffer[0] = Package::ErrorRegister;
				bgSendData(1, buffer);				
			}else{
				ofile.open(filename.c_str());
				ofile << "name=" << _name << "\npassw=" << _password << endl;
				ofile << "mail=" << _mail << endl;
				ofile << "deck=0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39\n";
				ofile << "allCards=0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44\n";
				ofile.close();
				char *buffer = new char[1];	
				buffer[0] = Package::OkRegister;
				bgSendData(1, buffer);
			}			
		}
	}
	delete n.data;
	return 0;
}

int Handle::quit(){
	if(player && player->user) cout << player->user->name << " termino\n";
	SDL_SemWait(waitListSemaphore);
	waitList.remove(this);
	SDL_SemPost(waitListSemaphore);
	if(player) delete player, player = NULL;	
	if(game) delete game, game = NULL;
	if(enemy) enemy->endMatch();
	
	return 0;
}

void Handle::bgSendData(size_t len, char *data)
{	
	packet n;
	n.len = len;
	n.data = data;
	
	SDL_SemWait(sendSem);
	sendQueue.push(n);
	SDL_SemPost(sendSem);
}

void Handle::setGame(Game *g)
{
	game = g;
}

int Handle::startBattle(Handle *ene)
{
	enemy  = ene;
	status = HandleStatus::Fighting;
	
	char *buffer = new char[1];	
	buffer[0] = Package::FoundEnemy;
	bgSendData(1, buffer);
	
	player->shuffleDeck();
	enemy->player->shuffleDeck();
	
	turnid = 1;
	
	cout << "Empieza pelea entre " << player->user->name << " y " << enemy->player->user->name << endl;
	return 0;
}

void Handle::sendCards(){
	if(!player){
		char *bufferErrorLogin = new char[1];
		bufferErrorLogin[0] = Package::ErrorLogin;
		bgSendData(1, bufferErrorLogin);
		return ;
	}
	
	if(!enemy){
		char *bufferErrorEnemy = new char[1];
		bufferErrorEnemy[0] = Package::OponentLeft;
		bgSendData(1, bufferErrorEnemy);
		return ;
	}
	
	for(int i=0;i<4;i++){
		char *buffer = new char[7];
		buffer[0] = Package::CardUpdate;

		buffer[1] = i;		
		if(player->getHand(i) != NULL){
			buffer[2] = player->getHand(i)->cardindex+1;
			buffer[3] = player->getHand(i)->energy;
			buffer[4] = player->getHand(i)->life;
			buffer[5] = player->getHand(i)->originalLife;
			buffer[6] = player->getHand(i)->attackAmount;
		}else{
			buffer[2] = 0;
		}
		bgSendData(7, buffer);		
	}
	/**Mi bicho**/
	char *buffer1 = new char[7];
	buffer1[0] = Package::CardUpdate;
	buffer1[1] = 4;
	if(player->getBattle() != NULL){
		buffer1[2] = player->getBattle()->cardindex+1;
		buffer1[3] = player->getBattle()->energy;
		buffer1[4] = player->getBattle()->life;
		buffer1[5] = player->getBattle()->originalLife;
		buffer1[6] = player->getBattle()->attackAmount;
	}else{
		buffer1[2] = 0;
	}
	bgSendData(7, buffer1);	
	
	/**Bicho del oponente**/
	char *buffer2 = new char[7];
	buffer2[0] = Package::CardUpdate;
	buffer2[1] = 5;
	if(enemy->getPlayer()->getBattle() != NULL){
		buffer2[2] = enemy->getPlayer()->getBattle()->cardindex+1;
		buffer2[3] = enemy->getPlayer()->getBattle()->energy;
		buffer2[4] = enemy->getPlayer()->getBattle()->life;
		buffer2[5] = enemy->getPlayer()->getBattle()->originalLife;
		buffer2[6] = enemy->getPlayer()->getBattle()->attackAmount;
	}else{
		buffer2[2] = 0;
	}
	bgSendData(7, buffer2);		
	
	char *buffer3 = new char[2];
	buffer3[0] = Package::EmptyUpdate;
	buffer3[1] = 0;
	
	if(player->isDeckEmpty())
		buffer3[1] |= 0x01;
	if(enemy->getPlayer()->isDeckEmpty())
		buffer3[1] |= 0x02;
	if(player->isDiscardEmpty())
		buffer3[1] |= 0x04;
	if(enemy->getPlayer()->isDiscardEmpty())
		buffer3[1] |= 0x08;	
	
	bgSendData(2, buffer3);
	
	if(game != NULL){
		char *buffer4 = new char[1];
		if(turnid == game->getTurn())
			buffer4[0] = Package::YourTurn;
		else
			buffer4[0] = Package::OponentTurn;
		bgSendData(1, buffer4);
	}
	
}

void Handle::endMatch()
{
	enemy = NULL;
	game = NULL;
	sendCards();
	waitFindOponent();
}

void Handle::waitFindOponent(){
	SDL_SemWait(waitListSemaphore);
	if(waitList.empty()){
		waitList.push_front(this);
		SDL_SemPost(waitListSemaphore);
		cout << player->user->name << ": Buscando enemigo..." << endl;
		status = HandleStatus::Waiting;
	}else{
		enemy = waitList.back();
		waitList.pop_back();
		SDL_SemPost(waitListSemaphore);
		
		char *buffer = new char[1];	
		buffer[0] = Package::FoundEnemy;
		bgSendData(1, buffer);
		
		enemy->startBattle(this);
		status = HandleStatus::Fighting;
		game   = new Game(player, enemy->player);
		enemy->setGame(game);		
		
		sendCards();
		enemy->sendCards();
		
	}
}
