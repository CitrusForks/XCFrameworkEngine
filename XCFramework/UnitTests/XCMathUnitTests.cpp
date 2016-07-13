#include "UnitTestPrecompiledHeader.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XCFrameworkUnitTest
{
    namespace XCMath
    {
        TEST_CLASS(XCFloat2)
        {
        public:

            TEST_METHOD(Operations)
            {
                XCVec2 vec1, vec2, expected;
                expected = XCVec2(1.0f, 2.0f);

                vec1.SetValues(0.5f, 1.0f);
                vec2.SetValues(0.5f, 1.0f);

                vec1 += vec2;
                Assert::IsTrue(vec1 == expected);

                vec1 -= vec2;
                Assert::IsTrue(vec1 == vec2);

                vec1 = vec1 * 2;
                Assert::IsTrue(vec1 == expected);

                vec1 = vec1 / XCVec2(2.0f, 2.0f);
                Assert::IsTrue(vec1 == vec2);

                vec1 = vec1 * XCVec2(2.0f, 2.0f);
                Assert::IsTrue(vec1 == expected);
            }

        };

        TEST_CLASS(XCFloat3)
        {
        public:

            TEST_METHOD(Operations)
            {
                XCVec3 vec1, vec2, expected;
                expected = XCVec3(1.0f, 2.0f, 4.0f);

                vec1.SetValues(0.5f, 1.0f, 2.0f);
                vec2.SetValues(0.5f, 1.0f, 2.0f);

                vec1 += vec2;
                Assert::IsTrue(vec1 == expected);

                vec1 -= vec2;
                Assert::IsTrue(vec1 == vec2);

                vec1 = vec1 * 2;
                Assert::IsTrue(vec1 == expected);

                vec1 = vec1 / XCVec3(2.0f, 2.0f, 0.0f);
                Assert::IsTrue(vec1 == vec2);

                vec1 = vec1 * XCVec3(2.0f, 2.0f, 0.0f);
                Assert::IsTrue(vec1 == expected);
            }

        };

        TEST_CLASS(XCFloat4)
        {
        public:

            TEST_METHOD(Operations)
            {
                XCVec4 vec1, vec2, expected;
                expected = XCVec4(1.0f, 2.0f, 0.0f, 0.0f);

                vec1.SetValues(0.5f, 1.0f, 0.0f, 0.0f);
                vec2.SetValues(0.5f, 1.0f, 0.0f, 0.0f);

                vec1 += vec2;
                Assert::IsTrue(vec1 == expected);

                vec1 -= vec2;
                Assert::IsTrue(vec1 == vec2);

                vec1 = vec1 * 2;
                Assert::IsTrue(vec1 == expected);

                vec1 = vec1 / XCVec4(2.0f, 2.0f, 0.0f, 0.0f);
                Assert::IsTrue(vec1 == vec2);

                vec1 = vec1 * XCVec4(2.0f, 2.0f, 0.0f, 0.0f);
                Assert::IsTrue(vec1 == expected);
            }

        };
    }
}