/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/System/ISystem.h"

class MemorySystem : public ISystem
{
public:
    DECLARE_OBJECT_CREATION(MemorySystem)

    MemorySystem() {}
    virtual ~MemorySystem() {}
    
    virtual void Init(int heapSize) { m_totalHeapSize = heapSize; }
    virtual void Destroy() {}

private:
    unsigned long               m_totalHeapSize;
};