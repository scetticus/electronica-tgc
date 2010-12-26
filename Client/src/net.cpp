#include "net.h"

int network::sendThreadFn(){
	while(1){
		SDL_SemWait(sendSem);
		if(!sendQueue.empty()){
			packet n = sendQueue.front();
			sendQueue.pop();
			SDL_SemPost(sendSem);
			if(sendData_blocking(sizeof(size_t), (const char*) &n.len) < 0) status = 0;
			if(sendData_blocking(n.len, n.data) < 0) status = 0;
			delete n.data;
		}else{
			SDL_SemPost(sendSem);
		}
		SDL_Delay(1);
	}
	return 0;
}
int network::recvThreadFn(){
	while(1){
		packet n;
		
		if(recvData_blocking(sizeof(size_t), (char*) &n.len) < 0) status = 0;
		n.data = new char[n.len];
		if(recvData_blocking(n.len, n.data) < 0) status = 0;
		
		SDL_SemWait(recvSem);
		recvQueue.push(n);
		SDL_SemPost(recvSem);
		
		SDL_Delay(1);
	}
	return 0;
}

int network::sendThreadHelper(void *_t){
	network *t = (network*) _t;
	return t->sendThreadFn();
}
int network::recvThreadHelper(void *_t){
	network *t = (network*) _t;
	return t->recvThreadFn();
}

network::network(){
	#ifdef WIN32
		WORD w = MAKEWORD(1,1);
		WSADATA wsadata;
		WSAStartup(w, &wsadata);
	#endif
	sockfd=0;
	
	sendSem = SDL_CreateSemaphore( 1 );
	recvSem = SDL_CreateSemaphore( 1 );
	status = 1;
}

network::~network(){
	close();
	SDL_DestroySemaphore(sendSem);
	SDL_DestroySemaphore(recvSem);	
	sockfd = 0;
}

int network::open(const string &hostname){	
    struct sockaddr_in addr;
	struct hostent* he;
	int sock;
	sock = socket(PF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	
	cout << "getHost(" << hostname.c_str() << ")" << endl;
	
	he = gethostbyname(hostname.c_str());
	if(he==NULL){
		cout << "gethost" << endl;
		#ifdef WIN32
			cout << "WSAError: " << WSAGetLastError() << endl;
		#endif
		return -1;
	}
	
	memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
	addr.sin_port = htons(NETPORT);
	
	cout << "getHostOK: " << inet_ntoa(addr.sin_addr) << endl;
	
	if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		cout << "connectando" << endl;
		#ifdef WIN32
			cout << "WSAError: " << WSAGetLastError() << endl;
		#endif
		return -1;
	}
	
	sockfd = sock;
	
	status = 1;
	sendThread = SDL_CreateThread(sendThreadHelper, this);
	recvThread = SDL_CreateThread(recvThreadHelper, this);	
	return 0;
}

int network::close(){
	status = 0;
	if (shutdown(sockfd, SD_SEND) < 0) return -1;
	
	char acReadBuffer[1024];
    while(1){
		int nNewBytes = recv(sockfd, acReadBuffer, 1024, 0);
		if (nNewBytes == SOCKET_ERROR)
			return -1;
		else if (nNewBytes != 0)
			cerr << "Recibidos " << nNewBytes << " bytes no esperados durante la desconexion" << endl;
		else
			break;
    }
	
    if (closesocket(sockfd) < 0 ) return -1;
	
	SDL_KillThread(sendThread);
	SDL_KillThread(recvThread);
    return 0;
}
	
int network::sendData_blocking(size_t len, const char *data){
	if(sockfd == 0) return -1;
	if(status == 0) return -1;
	size_t sent=0;
	
	do{
		int r = send(sockfd,data+sent,len-sent,0);
		if(r<=0) return -2;
		sent += r;
	}while(sent<len);
	
	return 0;
}

int network::recvData_blocking(size_t len, char *data){
	if(sockfd == 0) return -1;
	if(status == 0) return -1;
	size_t recpt=0;
	
	do{
		int r = recv(sockfd,data+recpt,len-recpt,0);
		if(r<=0) return -2;
		recpt += r;
	}while(recpt<len);
	
	return 0;
}

int network::sendData(size_t len, const char *data){	
	char *copy = new char[len];
	memcpy(copy, data, len);
	
	packet n;
	n.len = len;
	n.data = copy;
	
	SDL_SemWait(sendSem);
	sendQueue.push(n);
	SDL_SemPost(sendSem);
	return 0;
}

packet network::recvData(){
	packet n;
	n.len = 0;
	n.data = NULL;
	
	SDL_SemWait(sendSem);
	
	if(!recvQueue.empty()){
		n = recvQueue.front();
		recvQueue.pop();
		SDL_SemPost(sendSem);
		return n;
	}else{
		SDL_SemPost(sendSem);
		return n;
	}
}

int network::recvEmpty(){
	SDL_SemWait(sendSem);
	int n = recvQueue.empty();
	SDL_SemPost(sendSem);
	return n;
}

int network::lostConn(){
	return 0;
}
