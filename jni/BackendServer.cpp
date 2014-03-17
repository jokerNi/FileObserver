#include "BackendServer.h"

#include <string>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <android/log.h>
#include "MyLog.h"
#include "SimpleTcpClient.h"
#include "jce_header/Observer.h"
#include "ProtocolUtil.h"
#include "FileDeleteObserver.h"

using namespace std;
using namespace Observer;

const int kSelectTimeout = 6;      // Seconds
const int kMaxRetryTimes = 5;

bool gKeepAliveDaemonProcess = true;
static BackendServer* sBackendServer = NULL;
static int sPort;
static string sUrl;
static string sGuid;
static string sVersion;

typedef void* (*ThreadProc)(void*);
int createThread(ThreadProc proc)
{
    bool success = false;

    pthread_t threadId;
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);

    success = !pthread_create(&threadId, &attributes, proc, NULL);

    pthread_attr_destroy(&attributes);

    return success;
}

void* BackendThread(void* params)
{
    XLOG("DaemonEchoThread start");

    sBackendServer = new BackendServer(sPort);
    sBackendServer->startInternal();

    XLOG("DaemonEchoThread end");
}

BackendServer::BackendServer(int port)
{
    mListenPort = port;
    mLoop = true;
    
    bzero(&mServerAddr, sizeof(mServerAddr));
    mServerAddr.sin_family = AF_INET;
    mServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    mServerAddr.sin_port = htons(mListenPort);

    createSocket();
    setupSocket();
    bindSocket();
}

bool BackendServer::isServerAlive(int port)
{
    SimpleTcpClient client;
    if (client.connect("127.0.0.1", port) < 0)
        return false;

    ControlMsg msg;
    msg.eCtrlType = E_CTRL_HELLO;
    msg.sSeq = "0";
    JceOutputStream<BufferWriter> os;
    msg.writeTo(os);
    client.write(os.getBuffer(), os.getLength());
    
    const int BUFFER_SIZE = 100;
    char buffer[BUFFER_SIZE] = {0};
    int length = 0;
    length = client.read(buffer, BUFFER_SIZE);
    XLOG("BackendServer::isServerAlive recv %s\n", buffer);

    ControlMsg recvMsg;
    JceInputStream<BufferReader> is;
    is.setBuffer(buffer, length);
    recvMsg.readFrom(is);

    XLOG("BackendServer::isServerAlive recv type = %d\n", recvMsg.eCtrlType);
    if (length > 0 && recvMsg.eCtrlType == E_CTRL_HELLO)
    {
        XLOG("BackendServer::isServerAlive return true");
        return true;
    }

    return false;
}

int BackendServer::start(int port, const char* path)
{
    XLOG("StartWatching begin");
    if (isServerAlive(port))
    {
        XLOG("BackendServer::start server is alive, return");
        return 0;
    }

    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        XLOG("fork failed");
    }
    else if (pid == 0)
    {
        XLOG("in new process, id is %d, ppid is %d", getpid(), getppid());
        XLOG("BackendServer::start path=%s", path);
        sPort = port;
        createThread(BackendThread);
        FileDeleteObserver observer(path);
        observer.setHttpRequestOnDelete(sUrl, sGuid, sVersion);
        observer.startWatching();

        while (gKeepAliveDaemonProcess)
        {
            //XLOG("StartWatching in while loop");

            usleep(1000 * 1000 * 2);
            //break;
            
            //XLOG("StartWatching leave while loop");
        }

        if (sBackendServer)
            sBackendServer->stop();

        XLOG("BackendServer::start exit");
        usleep(1000 * 1000 * 10);   // Wait a while for other components finish exist
        exit(0);
    }
    else
    {
        XLOG("in origin process, id is %d", getpid());
    }
    return 0;
}

void BackendServer::stop()
{
    gKeepAliveDaemonProcess = false;
}

void BackendServer::setData(std::string url, std::string guid, std::string version)
{
    sUrl = url;
    sGuid = guid;
    sVersion = version;
}

void BackendServer::startInternal()
{
    XLOG("BackendServer::startInternal begin");
    if (listen(mServerSocket, 60) < 0)
    {
        XLOG("listen failed");
        return;
    }

    int communicateSocket;
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    fd_set readFds;
    struct timeval timeout;
    int failTimes = 0;
    
    while (mLoop && (failTimes < kMaxRetryTimes))
    {
        //XLOG("BackendServer::start enter while loop\n");
        FD_ZERO(&readFds);
        FD_SET(mServerSocket, &readFds);
        int maxFd = mServerSocket + 1;
        timeout.tv_sec = kSelectTimeout;
        timeout.tv_usec = 0;
        
        switch (select(maxFd, &readFds, NULL, NULL, &timeout))
        {
            case -1:    // Error
                failTimes++;
                break;
            case 0:
                XLOG("BackendServer::startInternal timeout, continue");
                break;
            default:
                if (FD_ISSET(mServerSocket, &readFds))
                {
                    socklen_t client_addr_len = sizeof(client_addr);
                    XLOG("BackendServer::startInternal server begin accept\n");
                    communicateSocket = accept(mServerSocket, (sockaddr*)&client_addr, &client_addr_len);
                    if (communicateSocket < 0)
                    {
                        XLOG("accept failed");
                        break;
                    }
                    else
                    {
                        XLOG("Client(IP: %s) connected.\n", inet_ntoa(client_addr.sin_addr));
                    }
        
                    const int BUFFERSIZE = 1024;
                    char buffer[BUFFERSIZE] = {0};
                    int recvMsgSize = 0;
        
                    XLOG("BackendServer::startInternal server begin recv\n");
                    recvMsgSize = recv(communicateSocket, buffer, BUFFERSIZE, 0);
                    if (recvMsgSize < 0)
                    {
                        XLOG("server recv msg failed");
                        break;
                    }
                    else if (recvMsgSize == 0)
                    {
                        XLOG("server recv finished\n");
                        break;
                    }
                    else
                    {
                        XLOG("BackendServer::startInternal server recv msg success: %s\n", buffer);
                        if (send(communicateSocket, buffer, recvMsgSize, 0) != recvMsgSize)
                        {
                            XLOG("server send msg failed");
                            break;
                        }
                    }
                    close(communicateSocket);
                }
        }
    }
    
    close(mServerSocket);
    XLOG("BackendServer::startInternal end");
}

void BackendServer::stopInternal()
{
    mLoop = false;
}

void BackendServer::createSocket()
{
    mServerSocket= socket(PF_INET, SOCK_STREAM, 0);
    if (mServerSocket < 0)
    {
        XLOG("create socket failed");
        return;
    }
}

void BackendServer::setupSocket()
{
    int opt = 1;
    setsockopt(mServerSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

void BackendServer::bindSocket()
{
    if (bind(mServerSocket, (sockaddr*)&mServerAddr, sizeof(mServerAddr)) < 0)
    {
        XLOG("bind socket failed");
    }
}


