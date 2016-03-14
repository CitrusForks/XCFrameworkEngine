/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#if defined(UNUSED)

// CChunkManager.cpp : Defines the entry point for the console application.
//

#include "MemorySystemWin32.h"

#define KB 1024

int _tmain(int argc, _TCHAR* argv[])
{
	MemorySystemWin32* chunkMgr = new MemorySystemWin32(32);
	chunkMgr->allocateChunk();
	//chunkMgr->allocateBytes('a', 5);
	//chunkMgr->allocateBytes<int>(32768);

	char* n = chunkMgr->newAlloc<char>('a');
	int* ptrToInt = chunkMgr->newAlloc<int>(10);
	double* ptrToDbl = chunkMgr->newAlloc<double>(5.5);

	printf("-----------%c-%d-%f------\n", *n, *ptrToInt, *ptrToDbl);
	chunkMgr->printChunkInformation();

	chunkMgr->deleteAlloc<double>(ptrToDbl);
	//chunkMgr->deleteAlloc<int>(ptrToInt);


	printf("--------------DELETED-------------------");

	chunkMgr->printChunkInformation();

	ptrToInt = chunkMgr->newAlloc<int>(20);
	//ptrToDbl = chunkMgr->newAlloc<double>(6.6);

	char* newChar = chunkMgr->newAlloc('c');
	int* newInteger = chunkMgr->newAlloc(77);

	printf("--------------REINSERTED-------------------");
	printf("-----------%c-%d-%d-%c-----\n", *n, *ptrToInt, *newInteger, *newChar);

	chunkMgr->printChunkInformation();

	system("pause");
    return 0;
}
#endif