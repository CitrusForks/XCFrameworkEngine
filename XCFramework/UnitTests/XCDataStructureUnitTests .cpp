/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "UnitTestPrecompiledHeader.h"

#include "Base/DataStructures/Trees/XCTree.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XCFrameworkUnitTest
{
    TEST_CLASS(XCTreeTest)
    {
    public:

        class Data : public NTreeNode<Data*, BreathFirstSearch>
        {
        public:
            Data(u32 val)
                : m_value(val)
            {}

            ~Data()
            {
                m_value = 0;
            }

            u32     m_value;
        };

        TEST_METHOD(TreeTest)
        {
            Data* data = XCNEW(Data)(10);
            
            XCTree<Data*>* tree = XCNEW((XCTree<Data*>)) (data);

            Data* lastNode;

            //Add 4 nodes to root node
            for (u32 index = 0; index < 4; ++index)
            {
                data = XCNEW(Data)(10 * (index + 1));
                Data* level1Nodes = tree->AddNode(data);

                //Add 5 nodes to this level node
                for (u32 subindex = 4; subindex < 10; ++subindex)
                {
                    if (index == 2)
                    {
                        lastNode = level1Nodes;
                        data = XCNEW(Data)(10 * (subindex + 1) + 1);
                    }
                    else
                        data = XCNEW(Data)(10 * (subindex + 1));
                    tree->AddNode(level1Nodes, data);
                }
            }

            data = XCNEW(Data)(200);
            Data* deleteThisNode = tree->AddNode(lastNode, data);

            tree->PrintAll();

            //Delete the last node in root
            tree->RemoveNode(deleteThisNode);

            ::Logger("After Removing node");
            tree->PrintAll();

            XCDELETE(tree);
        }

    };
}