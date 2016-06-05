/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "SystemContainer.h"
#include "SystemFactory.h"

class SystemLocator
{
public:
    static SystemLocator*               GetInstance();
    ~SystemLocator();

    ISystem&                            RequestSystem(std::string sysName);

    template<class T>
    T&                                  RequestSystem(std::string sysName)
    {
        return (T&)m_systemContainer.GetSystem(sysName);
    }

    SystemContainer&                    GetSystemContainer() { return m_systemContainer; }

    void                                Destroy();
private:
    SystemLocator();

    static SystemLocator*               ms_pSystemLocator;

    SystemContainer                     m_systemContainer;
    std::unique_ptr<SystemFactory>      m_systemFactory;
};