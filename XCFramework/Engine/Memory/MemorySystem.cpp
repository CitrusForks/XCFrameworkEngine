/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"
#include "MemorySystem.h"

MemorySystem* MemorySystem::ms_pMemorySystem = nullptr;

MemorySystem::MemorySystem()
    : m_chunkSize(0)
{
}

void MemorySystem::Init(u64 chunkSize)
{
    ISystem::Init();
    ms_pMemorySystem = this;
}