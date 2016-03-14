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
    ~Server(void);
    
    void            init(const char *ip, int port);

    void            clientSocket();
    void            set_fd();
    void            sendData(const char* msg, int size = -1);
    char*           receiveData();

protected:
    void            createSocket(const char *ip, int port);
    void            bindSocket();

private:

    sockaddr_in     m_otherSocketAddress;
    SOCKET          m_otherSocket;
};