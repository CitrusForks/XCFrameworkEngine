/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "INetPeer.h"

class Client : public INetPeer
{
public:
    Client(void);
    ~Client(void);

    virtual void    init(const char *ip, int port);
    virtual void    sendData(const char*, int size = -1);
    virtual char*   receiveData();

    void            connectToServer();

protected:
    void    createSocket();
    void    set_fd();

    bool    m_isConnectedToServer;
};