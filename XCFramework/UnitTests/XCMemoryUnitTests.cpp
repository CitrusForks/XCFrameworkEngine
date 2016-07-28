/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "UnitTestPrecompiledHeader.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XCFrameworkUnitTest
{
    class A
    {
    public:
        A(int val)
        {
            m_integer = val;
            ::Logger("Ctor called");
        }

        ~A()
        {
            ::Logger("Dtor called");
        }

    private:
        int     m_integer;
    };

    TEST_CLASS(XCMemory)
    {
    public:

        TEST_METHOD(AllocDealloc)
        {
            MemorySystemWin32 memorySystem((u32)1024 * 1024);
            memorySystem.AllocateChunk();

            A* a = XCNEW(A)(10);
            Assert::IsTrue(MemorySystemWin32::GetInstance()->IsInMyMemory((uintptr_t)a) == true);
            XCDELETE(a);

            A* ab = XCNEW(A)(20);
            Assert::IsTrue(MemorySystemWin32::GetInstance()->IsInMyMemory((uintptr_t)ab) == true);
            XCDELETE(ab);

            A* abc = new A(20);
            Assert::IsTrue(MemorySystemWin32::GetInstance()->IsInMyMemory((uintptr_t)abc) == false);
            XCDELETE(abc);
            
            {
                std::vector<A*> listOfStackAllocs;
                listOfStackAllocs.push_back(XCNEW(A)(10));
                Assert::IsTrue(MemorySystemWin32::GetInstance()->IsInMyMemory((uintptr_t)listOfStackAllocs.back()) == true);
                XCDELETE(listOfStackAllocs.back());
                listOfStackAllocs.clear();
            }

            {
                std::vector<A> listOfStackAllocs;
                listOfStackAllocs.push_back(A(30));
                Assert::IsTrue(MemorySystemWin32::GetInstance()->IsInMyMemory((uintptr_t)&listOfStackAllocs.back()) == false);
                listOfStackAllocs.clear();
            }

            {
                std::vector<A*> listOfStackAllocs;
                listOfStackAllocs.push_back(new A(30));
                Assert::IsTrue(MemorySystemWin32::GetInstance()->IsInMyMemory((uintptr_t)listOfStackAllocs.back()) == false);
                XCDELETE(listOfStackAllocs.back());
                listOfStackAllocs.clear();
            }
        }

    };
}