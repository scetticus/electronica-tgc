#ifndef NET_H
#define NET_H

#include "common.h"

#ifdef WIN32
	#include <winsock2.h>
#endif

class network{
	public:
		int open(const string &hostname);
		int close();
		int sendData_blocking(size_t len, const char *data);
		int recvData_blocking(size_t len, char *data);
		
		int sendData(size_t len, const char *data);		
		packet recvData();
		int recvEmpty();
		int lostConn();
		
		network();
		~network();
	private:
		int sockfd;
		queue <packet>sendQueue;
		queue <packet>recvQueue;
		SDL_Thread *sendThread, *recvThread;
		SDL_sem *sendSem, *recvSem;
		
		static int sendThreadHelper(void *_t);
		static int recvThreadHelper(void *_t);
		
		int recvThreadFn();
		int sendThreadFn();
		
		int status;
};

#endif
