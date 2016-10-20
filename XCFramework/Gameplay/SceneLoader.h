/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "GameActors/GameActorsFactory.h"

class SceneLoader : public AsyncTask
{
public:
    SceneLoader(SceneGraph& world, std::string packageData);
    virtual ~SceneLoader();

    virtual void            Init();
    virtual void            Run();

protected:

    template<class Type>
    void                    LoadBuffer(Type buffer, std::string actorName);

    void                    LoadTree(const flatbuffers::Vector<flatbuffers::Offset<FBSceneNode>>* nodeList);
    const flatbuffers::Vector<flatbuffers::Offset<FBSceneNode>>* GetNodeList(const FBSceneNode* node);

    std::string             GetActorName(const FBSceneNode* node);

private:
    std::string                 m_packageName;
    SceneGraph&                 m_worldRef;
    GameActorsFactory*          m_actorFactory;
    FlatBuffersSystem::FBBuffer m_fbBuffer;
};

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
                m_worldRef.RequestAddActor(actor);
            }
        }
    }
}