/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/System/ISystem.h"

//Memory system needs to be a very bad singleton, its not even a singleton, it has a public ctor. 
//As it is used by the new/delete overrides, by that time, SystemLocator is not initialzed.
//Its a hybrid singleton. In this I could have the MemorySystem inside SystemContainer and can be located using SystemLocator.
class MemorySystem : public ISystem
{
public:
    DECLARE_OBJECT_CREATION(MemorySystem)

    MemorySystem();
    virtual ~MemorySystem() {}
    
    virtual void  Init(u64 chunkSize);

    virtual void  Update(f32 dt) {}
    virtual void* NewAlloc(size_t size) { return nullptr; }
    virtual void  DeleteAlloc(void** t) {}
    virtual bool  IsInMyMemory(uintptr_t address) { return false; }

    virtual void  Destroy() {}

    static MemorySystem* GetInstance() { return ms_pMemorySystem; }

protected:
    u64                     m_chunkSize;

private:
    static MemorySystem*    ms_pMemorySystem;
};