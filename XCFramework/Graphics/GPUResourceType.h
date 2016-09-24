/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

enum GPUResourceType
{
    GPUResourceType_RTV,
    GPUResourceType_DSV,
    GPUResourceType_SRV,
    GPUResourceType_CBV,
    GPUResourceType_UAV
};

class GPUResourceDesc
{
public:
    GPUResourceDesc(GPUResourceType type, u32 size)
        : m_bufferType(type), m_bufferSize(size)
    {
    }

    GPUResourceType   m_bufferType;
    u32               m_bufferSize;
};