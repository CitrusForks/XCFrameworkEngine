/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "SceneLoader.h"
#include "SceneGraph.h"

#include "Engine/GameplayBase/Actors/GameActorsFactory.h"


SceneLoader::SceneLoader(SceneGraph& world, std::string packageData)
    : AsyncTask("WorldSceneLoader")
    , m_packageName(packageData)
    , m_sceneRef(world)
{
}

SceneLoader::~SceneLoader()
{
}

void SceneLoader::Init()
{
    AsyncTask::Init();

    GameActorsFactory& actorFactory = SystemLocator::GetInstance()->RequestSystem<GameActorsFactory>("GameActorsFactory");
    m_actorFactory = &actorFactory;
}

void SceneLoader::Run()
{
    AsyncTask::Run();

    FlatBuffersSystem& fbSystem = SystemLocator::GetInstance()->RequestSystem<FlatBuffersSystem>("FlatBuffersSystem");
    fbSystem.ParseAndLoadFile(SCENE_SCHEMA_FILEPATH, m_fbBuffer);
    fbSystem.ParseAndLoadFile(m_packageName, m_fbBuffer);

    auto fbSceneGraph = GetFBSceneGraph(m_fbBuffer.GetBufferFromLoadedData());
    LoadNodeList<FBSceneNode>(m_sceneRef.GetSceneGraphTree().GetRootNode(), fbSceneGraph->SceneNodes());

    Destroy();
}

IActor* SceneLoader::ActorSpawner(std::string actorName, const void* nodeInstance)
{
    GameActorsFactory& actorFactory = SystemLocator::GetInstance()->RequestSystem<GameActorsFactory>("GameActorsFactory");

    IActor* actor = actorFactory.CreateActor(actorName);
    if (actor)
    {
        actor->PreLoad(nodeInstance);
    }

    return actor;
}

std::string SceneLoader::GetActorName(const FBSceneNode* node)
{
    //We remove the prefix "FB" from the enum names.
    std::string nodeName = EnumNameFBGenericSceneNode(node->NodeInstance_type());
    XCASSERT(strcmp(nodeName.substr(0, 2).c_str(), "FB") == 0);

    nodeName = nodeName.substr(2, nodeName.length() - 2);
    Logger("[SceneLoader]ActorName %s", nodeName.c_str());

    return nodeName;
}