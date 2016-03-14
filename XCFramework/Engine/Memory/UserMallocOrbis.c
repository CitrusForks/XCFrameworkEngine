#pragma once

#include <stdlib.h>
#include <mspace.h>
#include <kernel.h>
#include <assert.h>

#define nullptr 0

const unsigned int gHeapSize = 1024 * 1024 * 1024;
const unsigned int gMemAlign = 2 * 1024 * 1024;
SceLibcMspace gMSpace = nullptr;
SceLibcMallocManagedSize gMHeapsize = {};
off_t gMemStart;

//Override builtin allocs Function declarations
int user_malloc_init(void);
int user_malloc_finalize(void);
void *user_malloc(size_t size);
void user_free(void *ptr);
void *user_calloc(size_t nelem, size_t size);
void *user_realloc(void *ptr, size_t size);
void *user_memalign(size_t boundary, size_t size);
int user_posix_memalign(void **ptr, size_t boundary, size_t size);
void *user_reallocalign(void *ptr, size_t size, size_t boundary);
int user_malloc_stats(SceLibcMallocManagedSize *mmsize);
int user_malloc_stats_fast(SceLibcMallocManagedSize *mmsize);
size_t user_malloc_usable_size(void *ptr);

int user_malloc_init(void)
{
    void* addressSpace = nullptr;

    int res = sceKernelAllocateDirectMemory(0, SCE_KERNEL_MAIN_DMEM_SIZE, gHeapSize, gMemAlign, SCE_KERNEL_WB_ONION, &gMemStart);

    assert(res >= 0);

    res = sceKernelMapDirectMemory(&addressSpace, gHeapSize, SCE_KERNEL_PROT_CPU_READ | SCE_KERNEL_PROT_CPU_WRITE, 0, gMemStart, gMemAlign);

    assert(res >= 0);

    //Alloc mspace
    gMSpace = sceLibcMspaceCreate("XC Framework Malloc", addressSpace, gHeapSize, 0);

    assert(gMSpace != nullptr);


    return res;
}

int user_malloc_finalize(void)
{
    int res = 0;

    if (gMSpace)
    {
        res = sceLibcMspaceDestroy(gMSpace);
        assert(res >= 0);
    }

    res = sceKernelReleaseDirectMemory(gMemStart, gHeapSize);
    assert(res >= 0);

    return res;
}

void* user_malloc(size_t size)
{
    return sceLibcMspaceMalloc(gMSpace, size);
}

void user_free(void* objPtr)
{
    sceLibcMspaceFree(gMSpace, objPtr);
}

void* user_calloc(size_t neleme, size_t size)
{
    return sceLibcMspaceCalloc(gMSpace, neleme, size);
}

void* user_realloc(void* objPtr, size_t size)
{
    return sceLibcMspaceRealloc(gMSpace, objPtr, size);
}

void *user_memalign(size_t boundary, size_t size)
{
    return sceLibcMspaceMemalign(gMSpace, boundary, size);
}

int user_posix_memalign(void **ptr, size_t boundary, size_t size)
{
    return sceLibcMspacePosixMemalign(gMSpace, ptr, boundary, size);
}

void *user_reallocalign(void *ptr, size_t size, size_t boundary)
{
    return sceLibcMspaceReallocalign(gMSpace, ptr, boundary, size);
}

//E Replace malloc_stats function.
int user_malloc_stats(SceLibcMallocManagedSize *mmsize)
{
    return sceLibcMspaceMallocStats(gMSpace, mmsize);
}

//E Replace malloc_stats_fast function.
int user_malloc_stats_fast(SceLibcMallocManagedSize *mmsize)
{
    return sceLibcMspaceMallocStatsFast(gMSpace, mmsize);
}

//E Replace malloc_usable_size function.
size_t user_malloc_usable_size(void *ptr)
{
    return sceLibcMspaceMallocUsableSize(ptr);
}