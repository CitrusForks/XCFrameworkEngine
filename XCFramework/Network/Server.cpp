/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "NetworkPrecompiledHeader.h"
#include "Server.h"

Server::Server(void)
{
}

void Server::Init(const char *ip, i32 port)
{
#if defined(WIN32)
    m_result = WSAStartup(MAKEWORD(2,2), &m_WsaData);

    if(m_result != 0)
    {
        Logger("WSASTARTUP FAILED ", m_result);
    }
    m_pFdSetDesc = XCNEW(fd_set)();
    m_pTimeOut   = XCNEW(TIMEVAL)();

    CreateSocket((char*)ip, port);
    BindSocket();
    ClientSocket();
#endif
}

Server::~Server(void)
{
#if defined(WIN32)
    closesocket(m_socket);
    WSACleanup();
#endif
}

void Server::CreateSocket(const char *ip, i32 port)
{
#if defined(WIN32)
    m_socket = socket(AF_INET,SOCK_DGRAM, 0);

    if(m_socket == INVALID_SOCKET)
    {
        Logger("Error at socket %d", WSAGetLastError());
    }
    else
        Logger("\nSocket successful");

    m_sockAddressInfo.sin_addr.s_addr = inet_addr(ip);
    m_sockAddressInfo.sin_family = AF_INET;
    m_sockAddressInfo.sin_port = htons(port);

    Set_fd();
    m_pTimeOut->tv_sec = 0;
    m_pTimeOut->tv_usec = 0;
#endif
}

void Server::Set_fd()
{
#if defined(WIN32)
    m_pFdSetDesc->fd_count = 1;
    m_pFdSetDesc->fd_array[0] = m_socket;
#endif
}

void Server::BindSocket()
{
#if defined(WIN32)
    //Bind the socket
    m_result = bind(m_socket, (SOCKADDR*)&m_sockAddressInfo, sizeof(m_sockAddressInfo));

    if(m_result == SOCKET_ERROR)
    {
        printf("bind failed %d", WSAGetLastError());
        closesocket(m_socket);
    }
    else
        printf("Bind successful");
#endif

}

void Server::ClientSocket()
{
#if defined(WIN32)
    m_otherSocket = INVALID_SOCKET;
#endif
}

char* Server::ReceiveData()
{
#if defined(WIN32)
    m_socketLength = sizeof(m_otherSocketAddress);

    Set_fd();

    m_result = select(0, m_pFdSetDesc, 0, 0, m_pTimeOut);

    if (m_result > 0)
    {
        m_result = recvfrom(m_socket, m_RecvBuffer, DEFAULT_BUFLEN, 0, (sockaddr*)&m_otherSocketAddress, &m_socketLength);
        m_RecvBuffer[m_result] = '\0';
    }

    if (m_result <= 0)
    {
        m_RecvBuffer[0] = '!';
    }
#endif
    return m_RecvBuffer;
}

void Server::SendData(const char* msg, i32 size)
{
#if defined(WIN32)
    Logger("Sending");
    
    m_result = sendto(m_socket, msg, DEFAULT_BUFLEN, 0, (sockaddr*)&m_otherSocketAddress, m_socketLength);

    if(m_result>0)
    {
        Logger("sent bytes: ", m_result);
    }
#endif
}
