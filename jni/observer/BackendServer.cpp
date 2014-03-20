#include "BackendServer.h"

#include <string>
#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include "MyLog.h"
#include "SimpleTcpClient.h"
#include "jce_header/Observer.h"
#include "ProtocolUtil.h"
#include "FileDeleteObserver.h"

using namespace std;
using namespace Observer;

const int kSelectTimeout = 6;      // Seconds
const int kMaxRetryTimes = 5;
const int kBufferSize = 4096;

static bool gKeepAliveDaemonProcess = true;
static BackendServer* sBackendServer = NULL; 
 
typedef void* (*ThreadProc)(void*);
static int createThread(ThreadProc proc, void* params)
{
    bool success = false;

    pthread_t threadId;
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);

    success = !pthread_create(&threadId, &attributes, proc, params);

    pthread_attr_destroy(&attributes);

    return success;
}

static void* BackendThread(void* params)
{
    int port = (int)params;
    sBackendServer = new BackendServer(port);
    sBackendServer->startListening();
}

BackendServer::BackendServer(int port)
{
    mListenPort = port;
    mLoop = true;
    mFileObserver = NULL;
    
    bzero(&mServerAddr, sizeof(mServerAddr));
    mServerAddr.sin_family = AF_INET;
    mServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    mServerAddr.sin_port = htons(mListenPort);

    createSocket();
    setupSocket();
    bindSocket();
}

bool BackendServer::IsServerAlive(int port)
{
    SimpleTcpClient client;
    if (client.connect("127.0.0.1", port) < 0)
        return false;

    string send;
    ControlMsg msg;
    msg.eCtrlType = E_CTRL_HELLO;
    msg.sSeq = "0";
    ProtocolUtil::writeTo(msg, send);
    client.write(send.c_str(), send.length());
    
    char buffer[kBufferSize] = {0};
    int length = client.read(buffer, kBufferSize);

    ControlMsg recvMsg;
    JceInputStream<BufferReader> is;
    is.setBuffer(buffer, length);
    recvMsg.readFrom(is);

    if (length > 0 && recvMsg.eCtrlType == E_CTRL_HELLO)
        return true;

    return false;
}

int BackendServer::Start(int port)
{
    XLOG("BackendServer::Start port=%d", port);
    if (IsServerAlive(port))
    {
        XLOG("Server is alive, return");
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
        createThread(BackendThread, (void*)port);
        
        while (gKeepAliveDaemonProcess)
        {
            usleep(1000 * 1000 * 2);
        }

        if (sBackendServer)
            sBackendServer->stopListening();

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

void BackendServer::Stop()
{
    gKeepAliveDaemonProcess = false;
}

int BackendServer::SendRequest(int port, const char* buffer, int length)
{
    if (buffer == NULL || length == 0)
        return -1;

    SimpleTcpClient client;
    if (client.connect("127.0.0.1", port) < 0)
        return -2;

    client.write(buffer, length);
    
    return 0;
} 

void BackendServer::startListening()
{
    XLOG("BackendServer::startListening begin");
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
                break;
            default:
                if (FD_ISSET(mServerSocket, &readFds))
                {
                    socklen_t client_addr_len = sizeof(client_addr);
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

                    char buffer[kBufferSize] = {0};
                    int recvMsgSize = recv(communicateSocket, buffer, kBufferSize, 0);
                    if (recvMsgSize < 0)
                    {
                        XLOG("server recv msg failed");
                        break;
                    }
                    else if (recvMsgSize == 0)
                    {
                        XLOG("server recv finished");
                        break;
                    }
                    else
                    {
                        handle(communicateSocket, buffer, recvMsgSize);
                    }
                    close(communicateSocket);
                }
        }
    }
    
    close(mServerSocket);
    XLOG("BackendServer::startListening end");
}

void BackendServer::stopListening()
{
    mLoop = false;
}

int BackendServer::handle(int commSock, const char* buf, int length)
{
    if (commSock < 0 || buf == NULL || length == 0)
        return 0;
    
    ControlMsg recvMsg;
    JceInputStream<BufferReader> is;
    is.setBuffer(buf, length);
    recvMsg.readFrom(is);

    XLOG("BackendServer::handle CtrlType=%d", recvMsg.eCtrlType);
    switch (recvMsg.eCtrlType)
    {
        case E_CTRL_HELLO:
            if (send(commSock, buf, length, 0) != length)
            {
                XLOG("send msg failed");
                break;
            }
            break;
        case E_CTRL_FILE_PATH:
            {
                FilePath filePath;
                JceInputStream<BufferReader> is;
                is.setBuffer(recvMsg.vbData);
                filePath.readFrom(is);
                XLOG("BackendServer::handle path=%s", filePath.sFilePath.c_str());
                if (mFileObserver)
                    mFileObserver->stopWatching();
                mFileObserver = new FileDeleteObserver(filePath.sFilePath);
                mFileObserver->startWatching();
            }
            break;
        case E_CTRL_REQ_ON_DEL:
            XLOG("BackendServer::handle mFileObserver=%p", mFileObserver);
            if (mFileObserver != NULL)
            {
                ReqOnDel req;
                JceInputStream<BufferReader> is;
                is.setBuffer(recvMsg.vbData);
                req.readFrom(is);
                XLOG("BackendServer::handle url=%s", req.sUrl.c_str());
                mFileObserver->setHttpRequestOnDelete(req.sUrl);
            }
            break;
        default:
            break;
    }
    
    return 0;
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


