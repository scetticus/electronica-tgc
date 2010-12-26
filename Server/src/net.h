#ifndef NET_H
#define NET_H

#ifdef WIN32
	#include <winsock2.h>
	typedef int socklen_t;
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>	
	#include <netdb.h>
	#define SD_RECV 0
	#define SD_SEND 1
	#define SOCKET int
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
#endif

int sendData(int sockfd, size_t len, const char *data);
int recvData(int sockfd, size_t len, char *data);



#endif
