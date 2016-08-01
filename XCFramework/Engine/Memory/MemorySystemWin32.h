/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complau32 with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "MemorySystem.h"

class MemorySystemWin32 : public MemorySystem
{
public:
    DECLARE_OBJECT_CREATION(MemorySystemWin32)

    static const u32                AlignmentBoundary   = 16;

    static const u32                NEW_MEM_PATTERN     = 0x0;
    static const u32                DELETE_MEM_PATTERN  = -41;

    //Add more info to this. Its like header to every allocated memory.
    struct AllocInfo
    {
        AllocInfo(u32 bytes)
        {
            m_nbOfBytes = bytes;
        }

        u32     m_nbOfBytes;
    };


    MemorySystemWin32();
    ~MemorySystemWin32();

    void                 Init(u64 chunkSize)   override;
    void                 Update(f32 dt)        override;
    void                 Destroy()             override;

    void*                NewAlloc(size_t size) override;
    void                 DeleteAlloc(void** t) override;

    bool                 IsInMyMemory(uintptr_t address) override;

    inline uintptr_t     AlignPointer(u32 alignSize, uintptr_t** ptr);
    void                 PrintChunkInformation();

protected:
    bool                 AllocateChunk();
    void*                AllocateBytes(size_t val);
    void                 AssignAndUpdateAllocatedList(u32 blockIndex, u32 sizeOfObj);

    //First fit algorithm used. Can be enhanced further to use different strategy best-fit.
    void*                FindBlock(u32 sizeOfType);
    u32                  FindBlockFromAllocated(u32 sizeOfType);

    void*                GetPointerToAllocatedBlock(u32 blockIndex);

private:

    std::vector<AllocInfo>     m_allocatedBytesList;
    u64                        m_freeSize;

    char*                      m_pChunkFront;  //Do not modify this
    char*                      m_pChunkBack;
};

uintptr_t MemorySystemWin32::AlignPointer(u32 alignSize, uintptr_t** ptr)
{
    return (uintptr_t)(ptr) + (alignSize - 1) & ~(alignSize - 1);
}