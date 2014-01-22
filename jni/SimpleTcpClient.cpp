#include "SimpleTcpClient.h"
#include <strings.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "MyLog.h"


SimpleTcpClient::SimpleTcpClient()
{
    mClientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mClientSocket < 0)
        XLOG("create client socket failed");

    bzero(&mClientAddr, sizeof(mClientAddr));
    mClientAddr.sin_family = AF_INET;
    mClientAddr.sin_addr.s_addr = htons(INADDR_ANY);
    mClientAddr.sin_port = htons(0);
}

int SimpleTcpClient::connect(const char* ip, int port)
{
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);
    socklen_t server_addr_len = sizeof(server_addr);
    if (::connect(mClientSocket, (sockaddr*)&server_addr, server_addr_len) < 0)
    {
        XLOG("connect failed");
        return -1;
    }
    return 0;
}

int SimpleTcpClient::write(const char* buffer, int length)
{
    return send(mClientSocket, buffer, length, 0);
}

int SimpleTcpClient::read(char* buffer, int length)
{
    return recv(mClientSocket, buffer, length, 0);
}

