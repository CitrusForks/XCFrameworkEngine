/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

class INetPeer
{
public:
    static const i32 DEFAULT_BUFLEN = 512;

    INetPeer();
    virtual ~INetPeer();

    virtual void     Init(const char *ip, i32 port) = 0;
    virtual void     update() {}
    virtual void     SendData(const char* msg, i32 size = -1) = 0;
    virtual char*    ReceiveData() = 0;

protected:
    char             m_RecvBuffer[DEFAULT_BUFLEN];
    i32              m_result;

    i32              m_socketLength;
    SOCKET           m_socket;

    fd_set*          m_pFdSetDesc;
    TIMEVAL*         m_pTimeOut;


    sockaddr_in      m_sockAddressInfo;

#if defined(WIN32)   
    WSADATA          m_WsaData;
#endif
};