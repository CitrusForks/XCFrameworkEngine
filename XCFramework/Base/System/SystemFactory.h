/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/Serializer/ObjectFactory.h"
#include "ISystem.h"

class SystemFactory : ObjectFactory, public ISystem
{
public:
    SystemFactory();
    virtual ~SystemFactory();

    void                            InitFactory()     override final;
    void                            DestroyFactory()  override final;

    ISystem*                        CreateSystem(std::string sysName);

    template<class T>
    void                            RegisterSystem(std::string systemName)
    {
        RegisterObject<T>(systemName);
    }

private:
    i32                             m_systemInstanceCount;
};