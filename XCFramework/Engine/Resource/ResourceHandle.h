/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "IResource.h"

class ResourceHandle
{
public:
    ResourceHandle()
    {
        m_refCount = 0;
        m_serializerBuffer = nullptr;
    }


    template<class Type>
    Type*               GetResource() { return static_cast<Type*>(m_Resource); }
    bool                IsValid()     { return m_Resource != nullptr; }

    u32        m_refCount;
    IResource*          m_Resource;
    const void*         m_serializerBuffer;
};