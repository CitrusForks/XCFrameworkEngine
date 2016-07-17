/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "NetworkPrecompiledHeader.h"
#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
    m_pNetPeers.clear();
}

NetworkManager::~NetworkManager(void)
{
}

void NetworkManager::AddNetPeer(INetPeer* netPeer)
{
    if (netPeer)
    {
        //client->createSocket((char*)IP_ADDRESS);
        m_pNetPeers.push_back(netPeer);
    }
}

void NetworkManager::Update()
{
    for (auto peer : m_pNetPeers)
    {
        peer->update();
    }
}

void NetworkManager::NetworkSendData(const char* _data, i32 size)
{
    for (auto peer : m_pNetPeers)
    {
        peer->SendData(_data, size);
    }
}

char* NetworkManager::NetworkReceiveData()
{
    char *pValue = nullptr;
    
    for (auto peer : m_pNetPeers)
    {
        pValue = peer->ReceiveData();

        if (pValue && pValue[0] != '!')
        {
            Logger("Received data : %s ",pValue);
        }
    }

    return pValue;
}

void NetworkManager::Destroy()
{
    u32 i = 0;
    while (i < m_pNetPeers.size())
    {
        delete(m_pNetPeers[i++]);
    }
}