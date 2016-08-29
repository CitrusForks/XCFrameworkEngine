/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complau32 with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"
#include "MemorySystemWin32.h"

MemorySystemWin32::MemorySystemWin32()
{
}

MemorySystemWin32::~MemorySystemWin32()
{
}

void MemorySystemWin32::Init(u64 chunkSize)
{
    m_threadLock.Create();

    m_chunkSize = chunkSize;
    m_freeSize = m_chunkSize;

    bool res = AllocateChunk();
    XCASSERT(res);

    if (!res)
    {
        abort();
    }

    res ? MemorySystem::Init(chunkSize) : false;
}

void MemorySystemWin32::Update(f32 dt)
{
}

void MemorySystemWin32::Destroy()
{
    if (m_pChunkFront)
    {
        _aligned_free(m_pChunkFront);
    }

    //Null it here. The system container will destruct the instance of memory system.
    ms_pMemorySystem = nullptr;

    m_threadLock.Release();
}

bool MemorySystemWin32::IsInMyMemory(uintptr_t address)
{
    return address >= (uintptr_t)m_pChunkFront && address < (uintptr_t)m_pChunkBack;
}

bool MemorySystemWin32::AllocateChunk()
{
    m_pChunkFront = (c8*)_aligned_malloc(sizeof(c8) * m_chunkSize, AlignmentBoundary);
    m_pChunkBack = m_pChunkFront + m_chunkSize;

    return m_pChunkFront ? true : false;
}

void* MemorySystemWin32::AllocateBytes(size_t size)
{
    m_threadLock.Enter();

    //Align the size to request for min Alignment boundary.
    //Note : On win32, the intrinsic functions requires the boundary to be 16 byte aligned and also the object that contains it need to be aligned.
    size = (size + (AlignmentBoundary - 1)) & ~(AlignmentBoundary - 1);

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

    //Get pointer to allocatedBlockNumber
    void* ptrToBlock = GetPointerToAllocatedBlock(allocatedBlockNumber);

    m_threadLock.Exit();

    if (ptrToBlock)
    {
        memset(ptrToBlock, NewMemPattern, size);

        m_freeSize = m_freeSize - size;

        //Logger("[Mem] Alloc %x\n", ptrToBlock);

        return (void*)ptrToBlock;
    }

    Logger("[NewAlloc] Error. Block not found");
    XCASSERT(false);

    return nullptr;
}

void* MemorySystemWin32::NewAlloc(size_t size)
{
    if (size > m_freeSize)
    {
        Logger("[Memory] OUT OF MEMORY");
        XCASSERT(false);

        u32* crashHere = nullptr;
        *crashHere;
        return 0;
    }

    return AllocateBytes(size);
}

void MemorySystemWin32::DeleteAlloc(void** ptrToBlock)
{
    void* t = (*ptrToBlock);

    u32 offset = (c8*)t - m_pChunkFront;
    u32 offsetAdded = 0;

    m_threadLock.Enter();

    //Find the above offset in the allocated list
    for (u32 i = 0; i < m_allocatedBytesList.size(); i++)
    {
        if (offsetAdded == offset)
        {
            //Found!!! Here the next allocated i.e i+1 is the block that is to be deleted.
            //Garbage the contents by size in allocated bytes list
            u32 allocatedBytes = std::abs((int)m_allocatedBytesList[i].m_nbOfBytes);

            memset(t, DeleteMemPattern, allocatedBytes);

            //Found the block, make it invalid by negating the block
            m_allocatedBytesList[i].m_nbOfBytes *= -1;

            //Logger("[Mem] DeAlloc %x\n", t);

            (*ptrToBlock) = nullptr;
            t = nullptr;

            break;
        }
        offsetAdded += std::abs((int)m_allocatedBytesList[i].m_nbOfBytes);
    }

    m_threadLock.Exit();
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

            if (m_allocatedBytesList[allocatedBytesIndex].m_nbOfBytes <= 0)
            {
                //That means it's a free block, see if it fits for sizeOfType, may be here the diff cases can be searched for. Best fit, first fit.
                if ((u32)std::abs((int)m_allocatedBytesList[allocatedBytesIndex].m_nbOfBytes) >= sizeOfType)
                {
                    //Found the block, return the address.
                    return pStart += offsetByte;
                }
            }

            offsetByte += m_allocatedBytesList[allocatedBytesIndex].m_nbOfBytes;
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
        if (m_allocatedBytesList[allocatedBytesIndex].m_nbOfBytes <= 0)
        {
            //That means it's a free block, see if it fits for sizeOfType, may be here the diff cases can be searched for. Best fit, first fit.
            if ((u32)std::abs((int)m_allocatedBytesList[allocatedBytesIndex].m_nbOfBytes) >= sizeOfType)
            {
                //Found the block, return the address.
                return allocatedBytesIndex;
            }
        }
        offsetByte += m_allocatedBytesList[allocatedBytesIndex].m_nbOfBytes;
    }

    //If we come here that means allocated byte list was traversed and found no block, so find new block at the end of the allocated list
    //Block not found, just return -1
    return -1;
}

void MemorySystemWin32::AssignAndUpdateAllocatedList(u32 blockIndex, u32 sizeOfObj)
{
    u32 blockSizeAllocated = std::abs((int)m_allocatedBytesList[blockIndex].m_nbOfBytes);

    if (sizeOfObj < blockSizeAllocated)
    {
        //This means, we need to divide the blocks and create a new entry for the invalid block and make it negative of available
        u32 dividedBlockSize = blockSizeAllocated - sizeOfObj;

        m_allocatedBytesList[blockIndex] = sizeOfObj;

        //Create new entry after the blockIndex and insert this divideBlockSize
        std::vector<AllocInfo>::iterator it = m_allocatedBytesList.begin() + blockIndex + 1;
        m_allocatedBytesList.insert(it, AllocInfo(dividedBlockSize * -1));
    }
    else
    {
        //Just update the block
        m_allocatedBytesList[blockIndex].m_nbOfBytes *= -1;
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

        offsetAdded += std::abs((int)m_allocatedBytesList[index].m_nbOfBytes);
    }

    return pStart + offsetAdded;
}
