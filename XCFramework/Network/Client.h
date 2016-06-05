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
    virtual ~Client(void);

    virtual void    Init(const char *ip, int port);
    virtual void    SendData(const char*, int size = -1);
    virtual char*   ReceiveData();

    void            ConnectToServer();

protected:
    void            CreateSocket();
    void            Set_fd();

    bool            m_isConnectedToServer;
};