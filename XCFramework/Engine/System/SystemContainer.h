/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "ISystem.h"
#include "SystemFactory.h"

class SystemContainer
{
public:
    INullSystem*          NULLSYSTEM;

    SystemContainer(void);
    virtual ~SystemContainer(void);

    void                  Init(SystemFactory& sysFactory);
    ISystem&              CreateNewSystem(std::string sysName);
    
    template<class T>
    T& CreateNewSystem(std::string sysName)
    {
        return (T&)CreateNewSystem(sysName);
    }
    
    void                  RemoveSystem(std::string sysName);
    ISystem&              GetSystem(std::string sysName);

    void                  Destroy();


    template<class T>
    void                  RegisterSystem(std::string sysName)
    {
        m_systemFactory->RegisterSystem<T>(sysName);
    }

private:

    std::map<std::string, ISystem*>     m_systemContainer;
    SystemFactory*                      m_systemFactory;
};