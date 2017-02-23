/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/System/ISystem.h"
#include "Base/Thread/CriticalSection.h"

class BaseIDGenerator : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(BaseIDGenerator);

    BaseIDGenerator();
    ~BaseIDGenerator();

    u32             GetNextBaseObjectId();

private:
    CriticalSection m_criticalSection;
    u32             m_baseObjectIDCounter;
};