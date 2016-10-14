/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "MeshNode.h"
#include "BoneHierarchy.h"

class AnimData
{
public:

};


class AnimNode
{
public:
    AnimNode()
    {
        m_channelIndex = -1;
        m_parentNode = nullptr;
    }

    AnimNode(std::string animName)
    {
        m_animName = animName;
        m_channelIndex = -1;
        m_parentNode = nullptr;
    }

    ~AnimNode()
    {
        while(m_childNodes.size() > 0)
        {
            XCDELETE(m_childNodes.back());
            m_childNodes.pop_back();
        }
    }

    i32                         m_channelIndex;

    XCMatrix4                   m_worldTransform;
    XCMatrix4                   m_localTransform;

    std::vector<AnimNode*>      m_childNodes;
    AnimNode*                   m_parentNode;
    std::string                 m_animName;
};

class MeshAnimator
{
public:

    MeshAnimator(const MeshNode* rootNode, const aiScene* scene)
    {
        m_rootNode = rootNode;
        m_scene = scene;
        m_currentNodeStructure = nullptr;
        m_nbOfAnimations = m_scene->mNumAnimations;

        PlayAnimation(0);
    }

    void                             PlayAnimation(u32 animIndex);
    void                             Update(f32 dt);
    std::vector<XCMatrix4Unaligned>& GetBoneMatrices(MeshNode* meshNode, BoneHierarchy* boneHierarchy, std::vector<BoneNode>& boneNodes);

protected:

    AnimNode*                        GenerateNodeStructure(const MeshNode* pNode, AnimNode* pParent);
    void                             CalculateWorldTransform(AnimNode* node);

    XCMatrix4                        GetWorldTransform(MeshNode* meshNode);
    XCMatrix4                        GetLocalTransform(MeshNode* meshNode);

private:

    u32                                   m_currentAnimationIndex;
    const MeshNode*                       m_rootNode;
    AnimNode*                             m_currentNodeStructure;
    std::map<const MeshNode*, AnimNode*>  m_mappedMeshNodeAnimNodes;
    std::vector<XCMatrix4Unaligned>       m_boneTransforms;
    u32                                   m_nbOfAnimations;

    //TODO : For now using aiScene for aiAnimation.
    const aiScene*                  m_scene;
};