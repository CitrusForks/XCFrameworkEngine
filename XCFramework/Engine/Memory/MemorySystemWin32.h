/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complau32 with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

class MemorySystemWin32
{
public:
    static const u32                AlignmentBoundary = 16;

    struct AllocInfo
    {
        AllocInfo(u32 bytes)
        {
            m_nbOfBytes = bytes;
        }

        u32     m_nbOfBytes;
    };

    static const u32                NEW_MEM_PATTERN     = 0x0;
    static const u32                DELETE_MEM_PATTERN  = -41;

    MemorySystemWin32(u32 chunkSize);
    ~MemorySystemWin32();

    static MemorySystemWin32*       GetInstance() { return ms_pMemorySystem; }

    bool                            AllocateChunk();
    void                            PrintChunkInformation();

    void*                           NewAlloc(size_t size);
    void                            DeleteAlloc(void** t);

    inline uintptr_t                AlignPointer(u32 alignSize, uintptr_t** ptr);

    bool                            IsInMyMemory(uintptr_t address);

    static MemorySystemWin32*       ms_pMemorySystem;

protected:
    void*                           AllocateBytes(size_t val);

    void*                           FindBlock(u32 sizeOfType);
    u32                             FindBlockFromAllocated(u32 sizeOfType);

    void                            AssignAndUpdateAllocatedList(u32 blockIndex, u32 sizeOfObj);

    void*                           GetPointerToAllocatedBlock(u32 blockIndex);

private:

    std::vector<AllocInfo>          m_allocatedBytesList;
    u32                             m_freeSize;
    u32                             m_chunkSize;

    char*                           m_pChunkFront;  //Do not modify this
    char*                           m_pChunkBack;
};

uintptr_t MemorySystemWin32::AlignPointer(u32 alignSize, uintptr_t** ptr)
{
    return (uintptr_t)(ptr) + (alignSize - 1) & ~(alignSize - 1);
}