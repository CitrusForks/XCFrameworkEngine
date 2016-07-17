/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"
#include "MeshAnimator.h"

void MeshAnimator::PlayAnimation(u32 animIndex)
{
    if (m_currentAnimationIndex == animIndex)
    {
        return;
    }

    if(m_currentNodeStructure != nullptr)
        delete m_currentNodeStructure;
    
    m_currentNodeStructure = GenerateNodeStructure(m_rootNode, nullptr);

    //TODO: AnimEvalutor
}

void MeshAnimator::Update(f32 dt)
{
}

AnimNode* MeshAnimator::GenerateNodeStructure(const MeshNode* pNode, AnimNode* pParent)
{
    // create a node
    AnimNode* internalNode = new AnimNode(pNode->m_nodeName);
    internalNode->m_parentNode = pParent;
    m_mappedMeshNodeAnimNodes[pNode] = internalNode;

    // copy its transformation
    internalNode->m_localTransform = pNode->m_transformation;
    CalculateWorldTransform(internalNode);

    // find the index of the animation track affecting this node, if any
    if (m_currentAnimationIndex < m_nbOfAnimations)
    {
        internalNode->m_channelIndex = -1;
        const aiAnimation* currentAnim = m_scene->mAnimations[m_currentAnimationIndex];
        for (u32 a = 0; a < currentAnim->mNumChannels; a++)
        {
            if (strcmp(currentAnim->mChannels[a]->mNodeName.C_Str(), internalNode->m_animName.c_str()) == 0)
            {
                internalNode->m_channelIndex = a;
                break;
            }
        }
    }

    // continue for all child nodes and assign the created internal nodes as our children
    for (u32 a = 0; a < pNode->m_childNodes.size(); a++)
    {
        AnimNode* childNode = GenerateNodeStructure(pNode->m_childNodes[a], internalNode);
        internalNode->m_childNodes.push_back(childNode);
    }

    return internalNode;
}

void MeshAnimator::CalculateWorldTransform(AnimNode* inNode)
{
    //Transfer all the transform of the parents into this node
    inNode->m_worldTransform = inNode->m_localTransform;

    AnimNode* parentNode = inNode->m_parentNode;
    while (parentNode != nullptr)
    {
        inNode->m_worldTransform = parentNode->m_localTransform * inNode->m_worldTransform;
        parentNode = parentNode->m_parentNode;
    }
}

std::vector<XCMatrix4Unaligned>& MeshAnimator::GetBoneMatrices(MeshNode* meshNode, BoneHierarchy* boneHierarchy, std::vector<BoneNode>& boneNodes)
{
    m_boneTransforms.resize(boneHierarchy->m_boneNodes.size(), MatrixTranspose(XCMatrix4::XCMatrixIdentity).GetUnaligned());

    XCMatrix4 worldInvTransform(GetWorldTransform(meshNode));
    worldInvTransform = MatrixInverse(worldInvTransform);

    // Bone matrices transform from mesh coordinates in bind pose to mesh coordinates in skinned pose
    // Therefore the formula is offsetMatrix * currentGlobalTransform * inverseCurrentMeshTransform
    for (u32 boneIndex = 0; boneIndex < boneHierarchy->m_boneNodes.size(); ++boneIndex)
    {
        const XCMatrix4& currentGlobalTransform(GetWorldTransform(boneHierarchy->m_boneNodes[boneNodes[boneIndex].m_boneName]));
        m_boneTransforms[boneIndex] = (MatrixTranspose(worldInvTransform * currentGlobalTransform * XCMatrix4(boneNodes[boneIndex].m_offsetMatrix))).GetUnaligned();
    }

    return m_boneTransforms;
}

XCMatrix4 MeshAnimator::GetWorldTransform(MeshNode* meshNode)
{
    auto it = m_mappedMeshNodeAnimNodes.find(meshNode);
    if (it != m_mappedMeshNodeAnimNodes.end())
    {
        return it->second->m_worldTransform;
    }

    return XCMatrix4::XCMatrixIdentity;
}

XCMatrix4 MeshAnimator::GetLocalTransform(MeshNode* meshNode)
{
    auto it = m_mappedMeshNodeAnimNodes.find(meshNode);
    if (it != m_mappedMeshNodeAnimNodes.end())
    {
        return it->second->m_localTransform;
    }

    return XCMatrix4::XCMatrixIdentity;
}