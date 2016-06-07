/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "SystemContainer.h"
#include "Engine/System/SystemFactory.h"

SystemContainer::SystemContainer(void)
{
}

SystemContainer::~SystemContainer(void)
{
}

void SystemContainer::Init(SystemFactory& sysFactory)
{
    m_systemFactory = &sysFactory;
    m_systemFactory->RegisterSystem<INullSystem>("INullSystem");
    
    NULLSYSTEM = (INullSystem*) m_systemFactory->CreateSystem("INullSystem");
}

ISystem& SystemContainer::CreateNewSystem(std::string sysName)
{
    if (m_systemContainer.find(sysName) == m_systemContainer.end())
    {
        m_systemContainer[sysName] = m_systemFactory->CreateSystem(sysName);
        Logger("[SystemContainer] Added new system %s", m_systemContainer[sysName]->GetSystemName().c_str());
    }
    else
    {
        XCASSERT(false);
        Logger("[SystemContainer] Already registered system %s", m_systemContainer[sysName]->GetSystemName().c_str());
    }

    return (*m_systemContainer[sysName]);
}

void SystemContainer::RemoveSystem(std::string sysName)
{
    auto res = m_systemContainer.find(sysName);
    
    if (res != m_systemContainer.end())
    {
        Logger("[SystemContainer] Removing system %s", res->second->GetSystemName().c_str());
        res->second->Destroy();
        m_systemContainer.erase(res->second->GetSystemName());
    }
    else
    {
        XCASSERT(false);
        Logger("[SystemContainer] Already Removed or not registered %s", sysName.c_str());
    }
}

ISystem& SystemContainer::GetSystem(std::string sysName)
{
    auto res = m_systemContainer.find(sysName);
    
    if (res != m_systemContainer.end())
    {
        res->second->ClientRequest();
        return (*res->second);
    }

    XCASSERT(false);
    return *NULLSYSTEM;
}

void SystemContainer::Destroy()
{
    for (auto it = m_systemContainer.begin(); it != m_systemContainer.end(); ++it)
    {
        if (it->second)
        {
            delete(it->second);
        }
    }

    m_systemContainer.clear();
    m_systemFactory->DestroyFactory();
}