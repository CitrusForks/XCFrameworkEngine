/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/GameplayBase/Actors/IActor.h"
#include "Assets/Packages/PackageConsts.h"

class ActorSerializer
{
public:

    static IActor*              ActorSpawner(std::string actorName, const void* nodeInstance);
    static std::string          GetActorName(const FBSceneNode* node);

    template<class NodeType>
    static void                 LoadNodeList(IActor* parentActor, const void* nodeList);

};

template<class NodeType>
void ActorSerializer::LoadNodeList(IActor* parentActor, const void* nodeList)
{
    const flatbuffers::Vector<flatbuffers::Offset<NodeType>>* typedNodeList = static_cast<const flatbuffers::Vector<flatbuffers::Offset<NodeType>>*>(nodeList);
    IActor* actor = nullptr;

    for (auto node = typedNodeList->begin(); node != typedNodeList->end(); ++node)
    {
        //Load the instance
        //...load all specifics to the node instance and its child from their respective nodelist
        actor = ActorSpawner(GetActorName(*node), node->NodeInstance());

        //Push to the parent and request for load.
        parentActor->AddNode(actor);
    }
}