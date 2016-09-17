/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "UnitTestPrecompiledHeader.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XCFrameworkUnitTest
{
    TEST_CLASS(XCTypes)
    {
    public:

        TEST_METHOD(TypeSizeTest)
        {
            Assert::IsTrue(sizeof(c8)  == 1);

            Assert::IsTrue(sizeof(u16) == 2);
            Assert::IsTrue(sizeof(u32) == 4);
            Assert::IsTrue(sizeof(u64) == 8);

            Assert::IsTrue(sizeof(i8)  == 1);
            Assert::IsTrue(sizeof(i16) == 2);
            Assert::IsTrue(sizeof(i32) == 4);
            Assert::IsTrue(sizeof(i64) == 8);

            Assert::IsTrue(sizeof(f32) == 4);
            Assert::IsTrue(sizeof(f64) == 8);

            Assert::IsTrue(sizeof(XCFloat2Unaligned) == sizeof(f32) * 2);
            Assert::IsTrue(sizeof(XCFloat3Unaligned) == sizeof(f32) * 3);
            Assert::IsTrue(sizeof(XCFloat4Unaligned) == sizeof(f32) * 4);
        }

    };
}