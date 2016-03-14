/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Serializer/IBase.h"

class ISystem : public IBase
{
public:
    ISystem()
    {
        m_NoRequestedClients = 0;
    }

    virtual ~ISystem(void);

    void                Init(std::string sysName) { m_sysName = sysName; }
    std::string         GetSystemName() { return m_sysName; }
    void                ClientRequest() { m_NoRequestedClients++; }
    virtual void        Destroy() {}

private:
    std::string         m_sysName;
    int                 m_NoRequestedClients;
};

class INullSystem : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(INullSystem)

    INullSystem()
    {}
};