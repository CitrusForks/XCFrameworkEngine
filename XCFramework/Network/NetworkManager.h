/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/System/ISystem.h"
#include "INetPeer.h"

class NetworkManager : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(NetworkManager)

    NetworkManager();
    ~NetworkManager(void);
    
    void                          AddNetPeer(INetPeer* peer);
    void                          RemoveNetPeer(INetPeer* peer);
                                  
    void                          Update();

    void                          NetworkSendData(const char* _data, i32 size = -1);
    char*                         NetworkReceiveData();
                                  
    void                          Destroy();

private:
    std::vector<INetPeer*>        m_pNetPeers;
};