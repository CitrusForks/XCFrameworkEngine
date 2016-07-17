/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "INetPeer.h"

class Server : public INetPeer
{
public:
    Server(void);
    virtual ~Server(void);
    
    void            Init(const char *ip, i32 port);

    void            ClientSocket();
    void            Set_fd();
    void            SendData(const char* msg, i32 size = -1);
    char*           ReceiveData();

protected:
    void            CreateSocket(const char *ip, i32 port);
    void            BindSocket();

private:

    sockaddr_in     m_otherSocketAddress;
    SOCKET          m_otherSocket;
};