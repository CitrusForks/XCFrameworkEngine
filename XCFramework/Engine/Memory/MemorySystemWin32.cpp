/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complau32 with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"
#include "MemorySystemWin32.h"

MemorySystemWin32* MemorySystemWin32::ms_pMemorySystem = nullptr;

MemorySystemWin32::MemorySystemWin32(u32 chunkSize)
{
    ms_pMemorySystem = this;

    //We know the size now.
    m_chunkSize = chunkSize;
    m_freeSize = m_chunkSize;
}

bool MemorySystemWin32::AllocateChunk()
{
    m_pChunkFront = (c8*)malloc(sizeof(c8) * m_chunkSize);
    m_pChunkBack = m_pChunkFront + m_chunkSize;

    return m_pChunkFront ? true : false;
}

void MemorySystemWin32::DeleteAlloc(void** ptrToBlock)
{
    void* t = (*ptrToBlock);

    u32 offset = (c8*)t - m_pChunkFront;
    u32 offsetAdded = 0;

    //Find the above offset in the allocated list
    for (u32 i = 0; i < m_allocatedBytesList.size(); i++)
    {
        if (offsetAdded == offset)
        {
            //Found!!! Here the next allocated i.e i+1 is the block that is to be deleted.
            //Garbage the contents by size in allocated bytes list
            u32 allocatedBytes = std::abs((int)m_allocatedBytesList[i]);

            memset(t, DELETE_MEM_PATTERN, allocatedBytes);

            //Found the block, make it invalid by negating the block
            m_allocatedBytesList[i] *= -1;

            (*ptrToBlock) = nullptr;
            t = nullptr;

            break;
        }
        offsetAdded += std::abs((int)m_allocatedBytesList[i]);
    }
}

void MemorySystemWin32::PrintChunkInformation()
{
    Logger("----------CHUNK INFO-----------\n");
    Logger("Address of Chunk : %x  to : %x\n", m_pChunkFront, (m_pChunkFront + m_chunkSize));
    Logger("Size of Chunk : %d\n", (m_pChunkFront + m_chunkSize) - m_pChunkFront);
    Logger("Allocated list: \n");

    for (u32 i = 0; i < m_allocatedBytesList.size(); i++)
    {
        Logger("%d \t", m_allocatedBytesList[i]);
    }

    Logger("\n----------Contents-------------\n");

    for (u32 i = 0; i < m_chunkSize; i++)
    {

        Logger("[%d] : %x : %d \t", i, &m_pChunkFront[i], m_pChunkFront[i]);

        if (i % 10 == 0)
        {
            Logger("\n");
        }
    }

    Logger("---------Free Size-------------\n %d bytes", m_freeSize);
}

void* MemorySystemWin32::FindBlock(u32 sizeOfType)
{
    //Move from the head ptr and find the best block
    c8* pStart = m_pChunkFront;

    u32 allocatedBytesIndex = 0;
    u32 offsetByte = 0;

    while (pStart <= m_pChunkBack)
    {
        //First check the allocated list and find a suitable block to insert
        if (allocatedBytesIndex < m_allocatedBytesList.size())
        {

            if (m_allocatedBytesList[allocatedBytesIndex] <= 0)
            {
                //That means it's a free block, see if it fits for sizeOfType, may be here the diff cases can be searched for. Best fit, first fit.
                if (std::abs((int)m_allocatedBytesList[allocatedBytesIndex]) >= sizeOfType)
                {
                    //Found the block, return the address.
                    return pStart += offsetByte;
                }
            }

            offsetByte += m_allocatedBytesList[allocatedBytesIndex];
            allocatedBytesIndex++;
            continue;
        }

        //If we come here that means allocated byte list was traversed and found no block, so find new block at the end of the allocated list
        if ((pStart + offsetByte + sizeOfType) <= m_pChunkBack)
        {
            return pStart + offsetByte;
        }
    }

    //Block not found
    return nullptr;
}

u32 MemorySystemWin32::FindBlockFromAllocated(u32 sizeOfType)
{
    u32 offsetByte = 0;

    for (u32 allocatedBytesIndex = 0; allocatedBytesIndex < m_allocatedBytesList.size(); allocatedBytesIndex++)
    {
        //First check the allocated list and find a suitable block to insert
        if (m_allocatedBytesList[allocatedBytesIndex] <= 0)
        {
            //That means it's a free block, see if it fits for sizeOfType, may be here the diff cases can be searched for. Best fit, first fit.
            if (std::abs((int)m_allocatedBytesList[allocatedBytesIndex]) >= sizeOfType)
            {
                //Found the block, return the address.
                return allocatedBytesIndex;
            }
        }
        offsetByte += m_allocatedBytesList[allocatedBytesIndex];
    }

    //If we come here that means allocated byte list was traversed and found no block, so find new block at the end of the allocated list
    //Block not found, just return -1
    return -1;
}

void MemorySystemWin32::AssignAndUpdateAllocatedList(u32 blockIndex, u32 sizeOfObj)
{
    u32 blockSizeAllocated = std::abs((int)m_allocatedBytesList[blockIndex]);

    if (sizeOfObj < blockSizeAllocated)
    {
        //This means, we need to divide the blocks and create a new entry for the invalid block and make it negative of available
        u32 dividedBlockSize = blockSizeAllocated - sizeOfObj;

        m_allocatedBytesList[blockIndex] = sizeOfObj;

        //Create new entry after the blockIndex and insert this divideBlockSize
        std::vector<u32>::iterator it = m_allocatedBytesList.begin() + blockIndex + 1;
        m_allocatedBytesList.insert(it, dividedBlockSize * -1);
    }
    else
    {
        //Just update the block
        m_allocatedBytesList[blockIndex] *= -1;
    }
}

void* MemorySystemWin32::GetPointerToAllocatedBlock(u32 blockIndex)
{
    u32 offsetAdded = 0;
    c8* pStart = m_pChunkFront;

    for (u32 index = 0; index < m_allocatedBytesList.size(); index++)
    {
        if (index == blockIndex)
        {
            break;
        }

        offsetAdded += std::abs((int)m_allocatedBytesList[index]);
    }

    return pStart + offsetAdded;
}

MemorySystemWin32::~MemorySystemWin32()
{
    free(m_pChunkFront);
}


//default new and delete overrides
void* operator new(size_t classSize)
{
    if (0/*MemorySystemWin32::ms_pMemorySystem*/)
    {
        //TODO : Support 16byte alignment in custom allocator.
        return MemorySystemWin32::ms_pMemorySystem->NewAlloc(classSize);
    }
    else
    {
        //Memory System not initialized yet.
        return _aligned_malloc(classSize, 16);
        //return malloc(classSize);
    }

}

void operator delete(void* p)
{
    if (0/*MemorySystemWin32::ms_pMemorySystem*/)
    {
        //TODO : Support 16byte alignment in custom allocator.
        return MemorySystemWin32::ms_pMemorySystem->DeleteAlloc(&p);
    }
    else
    {
        //Memory System not initialized yet.
        return _aligned_free(p);
        //return free(p);
    }
}