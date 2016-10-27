/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/TaskManager/Task/AsyncTask.h"
#include "Engine/GameplayBase/Actors/IActor.h"

#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"
#include "Assets/Packages/PackageConsts.h"

class GameActorsFactory;
class SceneGraph;

class SceneLoader : public AsyncTask
{
public:
    SceneLoader(SceneGraph& world, std::string packageData);
    virtual ~SceneLoader();

    virtual void            Init();
    virtual void            Run();

    static IActor*          ActorSpawner(std::string actorName, const void* nodeInstance);
    static std::string      GetActorName(const FBSceneNode* node);
    
    template<class NodeType>
    static void             LoadNodeList(IActor* parentActor, const void* nodeList);

protected:

    template<class Type>
    void                    LoadBuffer(Type buffer, std::string actorName);

private:
    std::string                 m_packageName;
    SceneGraph&                 m_sceneRef;
    GameActorsFactory*          m_actorFactory;
    FlatBuffersSystem::FBBuffer m_fbBuffer;
};

template<class NodeType>
void SceneLoader::LoadNodeList(IActor* parentActor, const void* nodeList)
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

template<class Type>
void SceneLoader::LoadBuffer(Type buffer, std::string actorName)
{
    IActor* actor = nullptr;

    if (buffer)
    {
        for (auto it = buffer->begin(); it != buffer->end(); ++it)
        {
            actor = m_actorFactory->CreateActor(actorName);
            if (actor)
            {
                actor->PreLoad(*it);

                //Push to world
                m_sceneRef.RequestAddActor(actor);
            }
        }
    }
}