#ifndef BackendServer_h
#define BackendServer_h

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

class BackendServer
{
public:
    BackendServer(int port);
    static bool isServerAlive(int port);
    static int start(int port, const char* path);
    static void stop();
    static void setData(std::string url, std::string guid, std::string version);
	void startInternal();
	void stopInternal();

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
