/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "MeshNode.h"

class BoneHierarchy
{
public:
    BoneHierarchy()
    {}

    ~BoneHierarchy()
    {
        for (auto node : m_boneNodes)
        {
            if (node.second)
                XCDELETE(node.second);
        }
        m_boneNodes.clear();
    }

    std::map<std::string, MeshNode*>  m_boneNodes;
};