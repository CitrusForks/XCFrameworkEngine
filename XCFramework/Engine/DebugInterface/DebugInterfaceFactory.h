/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/Serializer/ObjectFactory.h"
#include "Base/System/ISystem.h"

class IDebugFeature;

class DebugInterfaceFactory : ObjectFactory, public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(DebugInterfaceFactory);

    DebugInterfaceFactory();
    ~DebugInterfaceFactory();

    void                            InitFactory();
    void                            DestroyFactory();

    void                            RegisterDebugFeatures();

    IDebugFeature*                  CreateDebugFeature(std::string userFriendlyName);
};