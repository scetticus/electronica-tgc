#include "net.h"
#include <cstdio>
int sendData(int sockfd, size_t len, const char *data){
	if(sockfd == 0) return -1;
	
	size_t sent=0;
	
	do{
		int r = send(sockfd,data+sent,len-sent,0);
		if(r<=0) return -2;
		sent += r;
	}while(sent!=len);
	
	return 0;
}

int recvData(int sockfd, size_t len, char *data){
	if(sockfd == 0) return -1;
	
	size_t recpt=0;
	
	do{
		int r = recv(sockfd,data+recpt,len-recpt,0);
		if(r<=0) return -2;
		recpt += r;
	}while(recpt!=len);
	
	return 0;
}
