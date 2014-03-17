#ifndef BackendServer_h
#define BackendServer_h

#include <sys/socket.h>
#include <arpa/inet.h>

class BackendServer
{
public:
    BackendServer(int port);
    static bool isServerAlive(int port);
    void start();
	void stop();

private:
    void createSocket();
    void setupSocket();
    void bindSocket();

	bool mLoop;
    int mListenPort;
    int mServerSocket;
    int mCommunicateSocket;
    sockaddr_in mServerAddr;
};


#endif
