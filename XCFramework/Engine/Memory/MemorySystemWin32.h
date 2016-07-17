/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complau32 with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

class MemorySystemWin32
{
public:
    static const u32                NEW_MEM_PATTERN     = 0x0;
    static const u32                DELETE_MEM_PATTERN  = -41;

    MemorySystemWin32(u32 chunkSize);
    ~MemorySystemWin32();

    bool                            AllocateChunk();
    void                            PrintChunkInformation();

    template<class Type>
    bool                            AllocateBytes(Type val);

    template<class Type>
    Type*                           NewAlloc(Type t);

    template<class Type>
    Type*                           NewAlloc(size_t size);

    void                            DeleteAlloc(void** t);

    static MemorySystemWin32*       ms_pMemorySystem;

protected:
    void*                           FindBlock(u32 sizeOfType);
    u32                             FindBlockFromAllocated(u32 sizeOfType);

    void                            AssignAndUpdateAllocatedList(u32 blockIndex, u32 sizeOfObj);

    void*                           GetPointerToAllocatedBlock(u32 blockIndex);

private:
    std::vector<u32>                m_allocatedBytesList;
    u32                             m_freeSize;
    u32                             m_chunkSize;

    char*                           m_pChunkFront;  //Do not modify this
    char*                           m_pChunkBack;
};

template<class Type>
bool MemorySystemWin32::AllocateBytes(Type val)
{
    //Calc the size of obj
    u32 sizeOfObj = sizeof(val);

    if (sizeOfObj > m_freeSize)
        return false;

    Type* iter = (Type*)m_pChunkFront;
    *iter = val;

    m_freeSize = m_freeSize - sizeOfObj;

    return true;
}

template<class Type>
Type* MemorySystemWin32::NewAlloc(Type t)
{
    //Calc the size of obj
    u32 sizeOfObj = sizeof(t);

    if (sizeOfObj > m_freeSize)
    {
        Logger("[Memory] OUT OF MEMORY");
        XCASSERT(false);
        return 0;
    }

    //Find the best block and assign it to the Type
    u32 allocatedBlockNumber = FindBlockFromAllocated(sizeOfObj);

    //Update the Chunk
    //add the no of allocated bytes to the allocated list
    if (allocatedBlockNumber == -1)
    {
        //This means we need to add the block at the end of allocated list, so pushback
        m_allocatedBytesList.push_back(sizeOfObj);
        allocatedBlockNumber = m_allocatedBytesList.size() - 1;
    }
    else
    {
        AssignAndUpdateAllocatedList(allocatedBlockNumber, sizeOfObj);
    }

    //Get pou32er to allocatedBlockNumber
    void* ptrToBlock = GetPointerToAllocatedBlock(allocatedBlockNumber);

    if (ptrToBlock)
    {
        Type* temp = (Type*)ptrToBlock;

        //Assign the value t to the newly allocated memory
        *temp = t;

        m_freeSize = m_freeSize - sizeOfObj;

        return temp;
    }

    Logger("[NewAlloc] Error. Block not found");
    getchar();

    return 0;
}

template<class Type>
Type* MemorySystemWin32::NewAlloc(size_t size)
{
    if (size > m_freeSize)
    {
        Logger("[Memory] OUT OF MEMORY");
        XCASSERT(false);
        return 0;
    }

    //Find the best block and assign it to the Type
    u32 allocatedBlockNumber = FindBlockFromAllocated(size);

    //Update the Chunk
    //add the no of allocated bytes to the allocated list
    if (allocatedBlockNumber == -1)
    {
        //This means we need to add the block at the end of allocated list, so pushback
        m_allocatedBytesList.push_back(size);
        allocatedBlockNumber = m_allocatedBytesList.size() - 1;
    }
    else
    {
        AssignAndUpdateAllocatedList(allocatedBlockNumber, size);
    }

    //Get pou32er to allocatedBlockNumber
    void* ptrToBlock = GetPointerToAllocatedBlock(allocatedBlockNumber);

    if (ptrToBlock)
    {
        memset(ptrToBlock, NEW_MEM_PATTERN, size);

        //Placement new here in the location
        //Type* newedPtr = new(ptrToBlock) Type();
        m_freeSize = m_freeSize - size;

        return (Type*)newedPtr;
    }

    Logger("[NewAlloc] Error. Block not found");
    XCASSERT(false);

    return nullptr;
}


//#define XCNEW(x)    MemorySystemWin32::ms_pMemorySystem->newAlloc<x>(sizeof(x))
//#define XCDELETE(x) MemorySystemWin32::ms_pMemorySystem->deleteAlloc((void**)&x)

#define XCNEW(x)        new x
#define XCDELETE(x)     delete x

void* operator new(size_t classSize);
void operator delete(void* p);