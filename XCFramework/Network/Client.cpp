/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "NetworkPrecompiledHeader.h"
#include "Client.h"

//#define DEBUG_NETWORK_LOGS

Client::Client(void)
{
#if defined(WIN32)
    //init Winsock
    m_result = WSAStartup(MAKEWORD(2,2), &m_WsaData);
    
    if(m_result!=0)
    {
#if DEBUG_NETWORK_LOGS
        Logger("WSASTARTUP FAILED ", m_result);
#endif
    }
    m_isConnectedToServer = false;
#endif
}


Client::~Client(void)
{
#if defined(WIN32)
    closesocket(m_socket);
    WSACleanup();
#endif
}

void Client::Init(const char *ip, i32 port)
{
#if defined(WIN32)
    memset(&m_sockAddressInfo, 0, sizeof(sockaddr_in));

    m_sockAddressInfo.sin_addr.s_addr = inet_addr(ip);
    m_sockAddressInfo.sin_family = AF_INET;
    m_sockAddressInfo.sin_port = htons(port);   //270512

    m_pFdSetDesc = XCNEW(fd_set)();
    m_pTimeOut = XCNEW(TIMEVAL)();

    CreateSocket();
    ConnectToServer();
#endif
}

void Client::ConnectToServer()
{
#if defined(WIN32)
    m_result = select(0, 0, m_pFdSetDesc, 0, m_pTimeOut);
    m_result = connect(m_socket, (struct sockaddr*)&m_sockAddressInfo, sizeof(m_sockAddressInfo));

    if (m_result != 0)
    {
        Logger("[Network Client] Connecting error : ", WSAGetLastError());
    }
    else
    {
        m_isConnectedToServer = true;
    }
#endif
}

void Client::CreateSocket()
{
#if defined(WIN32)
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(m_socket == INVALID_SOCKET)
        Logger("[Network Client] Socket error ", WSAGetLastError());
    else
        Logger("[Network Client] success");

    Set_fd();

    m_pTimeOut->tv_sec = 0;
    m_pTimeOut->tv_usec = 0;
#endif
}

void Client::Set_fd()
{
#if defined(WIN32)
    m_pFdSetDesc->fd_count = 1;
    m_pFdSetDesc->fd_array[0] = m_socket;
#endif
}

char* Client::ReceiveData()
{
#if defined(WIN32)
    m_socketLength = sizeof(m_sockAddressInfo);

    Set_fd();
    m_result = select(0, m_pFdSetDesc, 0, 0, m_pTimeOut);

    if (m_result > 0)
    {
        m_result = recvfrom(m_socket, m_RecvBuffer, DEFAULT_BUFLEN, 0, (sockaddr*)&m_sockAddressInfo, &m_socketLength);
        m_RecvBuffer[m_result] = '\0';
    }

    if (m_result <= 0)
    {
        m_RecvBuffer[0] = '!';
    }
#endif
    return m_RecvBuffer;
}

void Client::SendData(const char* msg, i32 size)
{
#if defined(WIN32)
    i32 length = strlen(msg);

    if (size != -1)
    {
        length = size;
    }

#if DEBUG_NETWORK_LOGS
    Logger("Send data : Length %d\n", length);
#endif

    while (length > 0)
    {
        m_result = sendto(m_socket, msg, length, 0, (sockaddr*)&m_sockAddressInfo, sizeof(m_sockAddressInfo));

        if (m_result < 0)
        {
#if DEBUG_NETWORK_LOGS
            Logger("Send failed %d", WSAGetLastError());
#endif
            break;
        }
        else
        {
            length -= m_result;
#if DEBUG_NETWORK_LOGS
            Logger("Bytes sent: %d \n", m_result);
#endif
        }
    }
#endif
}
