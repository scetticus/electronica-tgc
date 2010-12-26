#include "common.h"
#include "serverHandle.h"
#include "net.h"

bool quit=false;	

bool shutdownConnection(u_int sd)
{
    if (shutdown(sd, SD_SEND) < 0) return false;
	
	char acReadBuffer[1024];
    while(1){
		int nNewBytes = recv(sd, acReadBuffer, 1024, 0);
		if (nNewBytes == SOCKET_ERROR)
			return false;
		else if (nNewBytes != 0)
			cerr << "Recibidos " << nNewBytes << " bytes no esperados durante la desconexion" << endl;
		else
			break;
    }
	#ifdef WIN32
		if (closesocket(sd) < 0 ) return false;
	#else
		if (close(sd) < 0 ) return false;
	#endif
    return true;
}

int dataHandler(void* sd_) 
{
	u_int sd = (u_int)sd_;
	
	Handle h(sd);
	while(h.handle()==0) ;
	h.quit();
	
    shutdownConnection(sd);
	cout << "Cerrada la conexion." << endl;
    
    return 0;
}

u_int setUpListener(int nPort)
{
	u_int sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd != INVALID_SOCKET) {
		sockaddr_in sinInterface;
		sinInterface.sin_family = AF_INET;
		sinInterface.sin_addr.s_addr = htons(INADDR_ANY);
		sinInterface.sin_port = nPort;
		if (bind(sd, (sockaddr*)&sinInterface, 
				sizeof(sockaddr_in)) != SOCKET_ERROR) {
			listen(sd, SOMAXCONN);
			return sd;
		}
		else {
			cerr << "Error en bind()" << endl;
		}
	}else{
		cerr << "Error en socket()" << endl;
	}
    return INVALID_SOCKET;
}

#ifdef WIN32

#include <windows.h>
#include <io.h>
#include <fcntl.h>

#endif

int main(int argc, char **args){
	srand(time(NULL));
	SDL_Init(SDL_INIT_EVERYTHING);
	freopen("info.txt", "w", stderr);

	#ifdef WIN32
		WORD w = MAKEWORD(1,1);
		WSADATA wsadata;
		WSAStartup(w, &wsadata);
		AllocConsole();
		freopen("CON", "wb", stdout);
//		freopen("CON", "wb", stderr);
		freopen("CON", "rb", stdin);
	#endif
	
	u_int Listeningu_int = setUpListener(htons(GAME_PORT));
	
	cout << "Servidor escuchando en puerto " << GAME_PORT << endl;
	
	sockaddr_in sinRemote;
    socklen_t nAddrSize = sizeof(sinRemote);

	waitListSemaphore = SDL_CreateSemaphore(1);
	
    while (!quit) {
		u_int sd = accept(Listeningu_int, (sockaddr*)&sinRemote, &nAddrSize);
        if(sd != INVALID_SOCKET){
            cout << "Nueva conexion de " << inet_ntoa(sinRemote.sin_addr) << endl;
			
			//int ms = 5000;setsockopt(sd, SOL_u_int, SO_RCVTIMEO, (const char*) &ms, sizeof(ms));
					
			SDL_Thread *newThread = SDL_CreateThread(dataHandler, (void*)sd);
			if(newThread == NULL){
				cerr << "Error creando thread" << endl;
			}
        }
    }
	
	SDL_DestroySemaphore(waitListSemaphore);
	SDL_Quit();
	#ifdef WIN32
		WSACleanup();
	#endif
	return 0;
}
