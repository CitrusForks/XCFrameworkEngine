#include "UnitTestPrecompiledHeader.h"

#include "Base/Memory/MemorySystem.h"
#include "Base/Memory/MemorySystemWin32.h"

#include "Gameplay/GameActors/IActor.h"
#include "Gameplay/GameActors/Soldier/Soldier.h"
#include "Gameplay/GameActors/GameActorsFactory.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XCFrameworkUnitTest
{
    namespace XCMath
    {
        TEST_CLASS(XCFloat2)
        {
        public:

            TEST_METHOD(XCFloat2Operations)
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

            TEST_METHOD(XCFloat3Operations)
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

            TEST_METHOD(XCFloat4Operations)
            {
                XCVec4 vec1, vec2, expected;
                expected = XCVec4(1.0f, 2.0f, 3.0f, 4.0f);

                vec1.SetValues(0.5f, 1.0f, 1.5f, 2.0f);
                vec2.SetValues(0.5f, 1.0f, 1.5f, 2.0f);

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

                //Test the object cotanining intrincis.
                SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
                container.RegisterSystem<MemorySystemWin32>("MemorySystem");
                container.RegisterSystem<GameActorsFactory>("GameActorsFactory");

                MemorySystem& memSys = (MemorySystem&)container.CreateNewSystem("MemorySystem");
                memSys.Init(1024 * 1024);

                IActor* actor = nullptr; //AP
                GameActorsFactory& actorFactory = (GameActorsFactory&)container.CreateNewSystem("GameActorsFactory");
                actorFactory.InitFactory();
                actor = actorFactory.CreateActor("Soldier");

                Assert::IsTrue(memSys.IsInMyMemory((uintptr_t)actor) == true);
            }

        };

        TEST_CLASS(XCMatrix)
        {
        public:

            TEST_METHOD(XCMatrixOperations)
            {
                XCMatrix matrix, expected;
                //matrix = matrix * matrix;
            }
        };
    }
}