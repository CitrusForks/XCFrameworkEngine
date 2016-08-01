/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#define XCNEW(x)         new (true) x
#define XCDELETE(x)      delete x
#define XCDELETEARRAY(x) delete[] x

//new overrides
void* operator new(size_t classSize);
void* operator new(size_t classSize, bool isNonMem);

void* operator new[](std::size_t size);
void* operator new[](size_t classSize, bool isNonMem);


//delete overrides
void operator  delete(void* p);
void operator  delete(void* p, bool isNonMem);
               
void operator  delete[](void* p);
void operator  delete[](void* p, bool isNonMem);