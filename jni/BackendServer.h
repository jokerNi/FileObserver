#ifndef BackendServer_h
#define BackendServer_h

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

class FileDeleteObserver;

class BackendServer
{
public:
    BackendServer(int port);
    static bool IsServerAlive(int port);
    static int Start(int port);
    static void Stop();
    static int SendRequest(int port, const char* buffer, int length);
    static void SetData(std::string url, std::string guid, std::string version);
    static int Port();

	void startListening();
	void stopListening();

private:
    int handle(int commSock, const char* buf, int length);
    void createSocket();
    void setupSocket();
    void bindSocket();

	bool mLoop;
    int mListenPort;
    int mServerSocket;
    int mCommunicateSocket;
    sockaddr_in mServerAddr;
    FileDeleteObserver* mFileObserver;
};


#endif
