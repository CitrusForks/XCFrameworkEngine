/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "AssimpIncludes.h"

class BoneInfo
{
public:
    BoneInfo(XCVec4Unaligned boneIndex, XCVec4Unaligned boneWeight)
        : m_boneIndices(boneIndex),
        m_boneWeight(boneWeight)
    {}

    XCVec4Unaligned m_boneIndices;
    XCVec4Unaligned m_boneWeight;
};

class BoneVertexWeight
{
public:
    BoneVertexWeight() {}
    BoneVertexWeight(const aiVertexWeight& weight)
    {
        m_vertexId = weight.mVertexId;
        m_weight = weight.mWeight;
    }

    BoneVertexWeight(u32 vertexId, f32 weight)
    : m_vertexId(vertexId), m_weight(weight)
    {}

    u32        m_vertexId;
    f32        m_weight;
};

class BoneNode
{
public:

    BoneNode(const aiBone* node)
    {
        m_boneName = node->mName.C_Str();
        for (u32 weightIndex = 0; weightIndex < node->mNumWeights; ++weightIndex)
        {
            m_vertexWeights.push_back(BoneVertexWeight(node->mWeights[weightIndex]));
        }
        m_offsetMatrix = aiMatrixToMatrix4Unaligned(node->mOffsetMatrix);
    }

    XCMatrix4Unaligned              m_offsetMatrix;
    std::vector<BoneVertexWeight>   m_vertexWeights;
    std::string                     m_boneName;
};
