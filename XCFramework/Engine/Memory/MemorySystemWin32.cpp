/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"
#include "MemorySystemWin32.h"

MemorySystemWin32* MemorySystemWin32::ms_pMemorySystem = nullptr;

MemorySystemWin32::MemorySystemWin32(int chunkSize)
{
    ms_pMemorySystem = this;

    //We know the size now.
    m_chunkSize = chunkSize;
    m_freeSize = m_chunkSize;
}

bool MemorySystemWin32::allocateChunk()
{
    m_pChunkFront = (char*)malloc(sizeof(char) * m_chunkSize);
    m_pChunkBack = m_pChunkFront + m_chunkSize;

    return m_pChunkFront ? true : false;
}

void MemorySystemWin32::deleteAlloc(void** ptrToBlock)
{
    void* t = (*ptrToBlock);

    int offset = (char*)t - m_pChunkFront;
    int offsetAdded = 0;

    //Find the above offset in the allocated list
    for (unsigned int i = 0; i < m_allocatedBytesList.size(); i++)
    {
        if (offsetAdded == offset)
        {
            //Found!!! Here the next allocated i.e i+1 is the block that is to be deleted.
            //Garbage the contents by size in allocated bytes list
            int allocatedBytes = abs(m_allocatedBytesList[i]);

            memset(t, DELETE_MEM_PATTERN, allocatedBytes);

            //Found the block, make it invalid by negating the block
            m_allocatedBytesList[i] *= -1;

            (*ptrToBlock) = nullptr;
            t = nullptr;

            break;
        }
        offsetAdded += abs(m_allocatedBytesList[i]);
    }
}

void MemorySystemWin32::printChunkInformation()
{
    Logger("----------CHUNK INFO-----------\n");
    Logger("Address of Chunk : %x  to : %x\n", m_pChunkFront, (m_pChunkFront + m_chunkSize));
    Logger("Size of Chunk : %d\n", (m_pChunkFront + m_chunkSize) - m_pChunkFront);
    Logger("Allocated list: \n");

    for (unsigned int i = 0; i < m_allocatedBytesList.size(); i++)
    {
        Logger("%d \t", m_allocatedBytesList[i]);
    }

    Logger("\n----------Contents-------------\n");

    for (unsigned int i = 0; i < m_chunkSize; i++)
    {

        Logger("[%d] : %x : %d \t", i, &m_pChunkFront[i], m_pChunkFront[i]);

        if (i % 10 == 0)
        {
            Logger("\n");
        }
    }

    Logger("---------Free Size-------------\n %d bytes", m_freeSize);
}

void* MemorySystemWin32::findBlock(int sizeOfType)
{
    //Move from the head ptr and find the best block
    char* pStart = m_pChunkFront;

    unsigned int allocatedBytesIndex = 0;
    int offsetByte = 0;

    while (pStart <= m_pChunkBack)
    {
        //First check the allocated list and find a suitable block to insert
        if (allocatedBytesIndex < m_allocatedBytesList.size())
        {

            if (m_allocatedBytesList[allocatedBytesIndex] <= 0)
            {
                //That means it's a free block, see if it fits for sizeOfType, may be here the diff cases can be searched for. Best fit, first fit.
                if (abs(m_allocatedBytesList[allocatedBytesIndex]) >= sizeOfType)
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

int MemorySystemWin32::findBlockFromAllocated(int sizeOfType)
{
    int offsetByte = 0;

    for (unsigned int allocatedBytesIndex = 0; allocatedBytesIndex < m_allocatedBytesList.size(); allocatedBytesIndex++)
    {
        //First check the allocated list and find a suitable block to insert
        if (m_allocatedBytesList[allocatedBytesIndex] <= 0)
        {
            //That means it's a free block, see if it fits for sizeOfType, may be here the diff cases can be searched for. Best fit, first fit.
            if (abs(m_allocatedBytesList[allocatedBytesIndex]) >= sizeOfType)
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

void MemorySystemWin32::assignAndUpdateAllocatedList(int blockIndex, int sizeOfObj)
{
    int blockSizeAllocated = abs(m_allocatedBytesList[blockIndex]);

    if (sizeOfObj < blockSizeAllocated)
    {
        //This means, we need to divide the blocks and create a new entry for the invalid block and make it negative of available
        int dividedBlockSize = blockSizeAllocated - sizeOfObj;

        m_allocatedBytesList[blockIndex] = sizeOfObj;

        //Create new entry after the blockIndex and insert this divideBlockSize
        std::vector<int>::iterator it = m_allocatedBytesList.begin() + blockIndex + 1;
        m_allocatedBytesList.insert(it, dividedBlockSize * -1);
    }
    else
    {
        //Just update the block
        m_allocatedBytesList[blockIndex] *= -1;
    }
}

void* MemorySystemWin32::getPointerToAllocatedBlock(int blockIndex)
{
    int offsetAdded = 0;
    char* pStart = m_pChunkFront;

    for (int index = 0; index < m_allocatedBytesList.size(); index++)
    {
        if (index == blockIndex)
        {
            break;
        }

        offsetAdded += abs(m_allocatedBytesList[index]);
    }

    return pStart + offsetAdded;
}

MemorySystemWin32::~MemorySystemWin32()
{
    free(m_pChunkFront);
}
