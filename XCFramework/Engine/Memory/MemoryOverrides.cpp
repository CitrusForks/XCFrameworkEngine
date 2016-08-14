/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "MemoryOverrides.h"
#include "MemorySystem.h"
#include "MemorySystemWin32.h"

void* NewAlloc(size_t classSize, bool isNonMem)
{
    MemorySystem* memSys = MemorySystem::GetInstance();
    if (isNonMem && memSys)
    {
        return memSys->NewAlloc(classSize);
    }
    else
    {
        return _aligned_malloc(classSize, MemorySystemWin32::AlignmentBoundary);
    }
}

void DeAlloc(void* ptr, bool isCustom)
{
    MemorySystem* memSys = MemorySystem::GetInstance();
    if (memSys && memSys->IsInMyMemory((uintptr_t)ptr))
    {
        memSys->DeleteAlloc(&ptr);
    }
    else
    {
        _aligned_free(ptr);
    }

    ptr = nullptr;
}

void* operator new(size_t classSize)
{
    return NewAlloc(classSize, false);
}

void* operator new[](std::size_t classSize)
{
    return NewAlloc(classSize, false);
}

void* operator new[](size_t classSize, bool isNonMem)
{
    return NewAlloc(classSize, isNonMem);
}

void* operator new(size_t classSize, bool isNonMem)
{
    return NewAlloc(classSize, isNonMem);
}

void operator delete(void* p)
{
    DeAlloc(p, false);
}

void operator delete(void* p, bool isNonMem)
{
    DeAlloc(p, true);
}

void operator delete[](void * p)
{
    DeAlloc(p, false);
}

void operator delete[](void * p, bool isNonMem)
{
    DeAlloc(p, true);
}
