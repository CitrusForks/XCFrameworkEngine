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
            delete m_childNodes.back();
            m_childNodes.pop_back();
        }
    }

    int                         m_channelIndex;

    XCMatrix4Unaligned          m_worldTransform;
    XCMatrix4Unaligned          m_localTransform;

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

    void                             PlayAnimation(unsigned int animIndex);
    void                             Update(float dt);
    std::vector<XCMatrix4Unaligned>& GetBoneMatrices(MeshNode* meshNode, BoneHierarchy* boneHierarchy, std::vector<BoneNode>& boneNodes);

protected:

    AnimNode*                        GenerateNodeStructure(const MeshNode* pNode, AnimNode* pParent);
    void                             CalculateWorldTransform(AnimNode* node);

    XCMatrix4Unaligned               GetWorldTransform(MeshNode* meshNode);
    XCMatrix4Unaligned               GetLocalTransform(MeshNode* meshNode);

private:
    unsigned int                    m_currentAnimationIndex;
    const MeshNode*                 m_rootNode;
    AnimNode*                       m_currentNodeStructure;
    std::map<const MeshNode*, AnimNode*>  m_mappedMeshNodeAnimNodes;
    std::vector<XCMatrix4Unaligned> m_boneTransforms;
    unsigned int                    m_nbOfAnimations;

    //TODO : For now using aiScene for aiAnimation.
    const aiScene*                  m_scene;
};