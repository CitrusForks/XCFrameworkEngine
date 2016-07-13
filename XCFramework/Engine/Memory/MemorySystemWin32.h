/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

class MemorySystemWin32
{
public:
    static const int                NEW_MEM_PATTERN     = 0x0;
    static const int                DELETE_MEM_PATTERN  = -41;

    MemorySystemWin32(int chunkSize);
    ~MemorySystemWin32();

    bool                            allocateChunk();
    void                            printChunkInformation();

    template<class Type>
    bool                            allocateBytes(Type val);

    template<class Type>
    Type*                           newAlloc(Type t);

    template<class Type>
    Type*                           newAlloc(size_t size);

    void                            deleteAlloc(void** t);

    static MemorySystemWin32*       ms_pMemorySystem;

protected:
    void*                           findBlock(int sizeOfType);
    int                             findBlockFromAllocated(int sizeOfType);

    void                            assignAndUpdateAllocatedList(int blockIndex, int sizeOfObj);

    void*                           getPointerToAllocatedBlock(int blockIndex);

private:
    std::vector<int>                m_allocatedBytesList;
    unsigned int                    m_freeSize;
    unsigned int                    m_chunkSize;

    char*                           m_pChunkFront;  //Do not modify this
    char*                           m_pChunkBack;
};

template<class Type>
bool MemorySystemWin32::allocateBytes(Type val)
{
    //Calc the size of obj
    int sizeOfObj = sizeof(val);

    if (sizeOfObj > m_freeSize)
        return false;

    Type* iter = (Type*)m_pChunkFront;
    *iter = val;

    m_freeSize = m_freeSize - sizeOfObj;

    return true;
}

template<class Type>
Type* MemorySystemWin32::newAlloc(Type t)
{
    //Calc the size of obj
    unsigned int sizeOfObj = sizeof(t);

    if (sizeOfObj > m_freeSize)
    {
        Logger("[Memory] OUT OF MEMORY");
        XCASSERT(false);
        return 0;
    }

    //Find the best block and assign it to the Type
    int allocatedBlockNumber = findBlockFromAllocated(sizeOfObj);

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
        assignAndUpdateAllocatedList(allocatedBlockNumber, sizeOfObj);
    }

    //Get pointer to allocatedBlockNumber
    void* ptrToBlock = getPointerToAllocatedBlock(allocatedBlockNumber);

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
Type* MemorySystemWin32::newAlloc(size_t size)
{
    if (size > m_freeSize)
    {
        Logger("[Memory] OUT OF MEMORY");
        XCASSERT(false);
        return 0;
    }

    //Find the best block and assign it to the Type
    int allocatedBlockNumber = findBlockFromAllocated(size);

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
        assignAndUpdateAllocatedList(allocatedBlockNumber, size);
    }

    //Get pointer to allocatedBlockNumber
    void* ptrToBlock = getPointerToAllocatedBlock(allocatedBlockNumber);

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