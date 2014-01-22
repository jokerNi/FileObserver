#ifndef EchoTcpServer_h
#define EchoTcpServer_h

#include <sys/socket.h>
#include <arpa/inet.h>

class EchoTcpServer
{
public:
    EchoTcpServer(int port);
    static bool isServerAlive(int port);
    void start();

private:
    void createSocket();
    void setupSocket();
    void bindSocket();

    int mListenPort;
    int mServerSocket;
    int mCommunicateSocket;
    sockaddr_in mServerAddr;
};


#endif
