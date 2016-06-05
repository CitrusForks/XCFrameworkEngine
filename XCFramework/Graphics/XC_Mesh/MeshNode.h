/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "AssimpIncludes.h"
#include "BoneInfo.h"

class MeshNode
{
public:
    MeshNode(const aiNode* node, const MeshNode* parentNode, bool tranverse = false)
    {
        m_nodeName = node->mName.C_Str();

        if (node->mParent)
        {
            m_parentNode = parentNode;
        }

        m_transformation = aiMatrixToMatrix4Unaligned(node->mTransformation);
        

        if (tranverse)
        {
            for (unsigned int childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
            {
                if (node->mChildren[childIndex])
                {
                    m_childNodes.push_back(new MeshNode(node->mChildren[childIndex], this, tranverse));
                }
            }
        }
    }

    MeshNode(const MeshNode* node, const MeshNode* parentNode, bool tranverse = false)
    {
        m_nodeName = node->m_nodeName;
        m_parentNode = node->m_parentNode;
        m_transformation = node->m_transformation;
        m_childNodes = node->m_childNodes;
    }

    ~MeshNode()
    {
        while (m_childNodes.size() > 0)
        {
            if (m_childNodes.back() != nullptr)
                delete(m_childNodes.back());
            m_childNodes.pop_back();
        }
    }

    std::string             m_nodeName;
    const MeshNode*         m_parentNode;
    XCMatrix4Unaligned      m_transformation;
    std::vector<MeshNode*>  m_childNodes;

    MeshNode* FindNode(std::string nodeName)
    {
        if (strcmp(nodeName.c_str(), m_nodeName.c_str()) == 0)
        {
            return this;
        }

        for (auto childNode : m_childNodes)
        {
            MeshNode* node = childNode->FindNode(nodeName);
            if (node)
            {
                return node;
            }
        }

        return nullptr;
    }
};