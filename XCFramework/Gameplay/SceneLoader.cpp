/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "SceneLoader.h"
#include "SceneGraph.h"
#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"
#include "Assets/Packages/PackageConsts.h"

SceneLoader::SceneLoader(SceneGraph& world, std::string packageData)
    : AsyncTask("WorldSceneLoader")
    , m_packageName(packageData)
    , m_worldRef(world)
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
    LoadTree((const flatbuffers::Vector<flatbuffers::Offset<FBSceneNode>>*)fbSceneGraph->SceneNodes());

    Destroy();
}

void SceneLoader::LoadTree(const flatbuffers::Vector<flatbuffers::Offset<FBSceneNode>>* nodeList)
{
    IActor* actor = nullptr;

    for (auto node = nodeList->begin(); node != nodeList->end(); ++node)
    {
        //Load the instance
        //...load all specifics to the node instance
        actor = m_actorFactory->CreateActor(GetActorName(*node));

        if (actor)
        {
            actor->PreLoad(node->NodeInstance());

            //Push to world
            m_worldRef.RequestAddActor(actor);
        }

        //check if any child node exists
        auto childList = GetNodeList(*node);

        if (childList)
        {
            LoadTree(childList);
        }
    }
}

std::string SceneLoader::GetActorName(const FBSceneNode* node)
{
    //We remove the prefix "FB" from the enum names.
    std::string nodeName = EnumNameFBGenericSceneNode(node->NodeInstance_type());
    XCASSERT(strcmp(nodeName.substr(0, 2).c_str(), "FB") == 0);

    nodeName = nodeName.substr(2, nodeName.length() - 2);
    Logger("[SceneLoader]ActorName %s", nodeName.c_str());

    return nodeName.c_str();
}

const flatbuffers::Vector<flatbuffers::Offset<FBSceneNode>>* SceneLoader::GetNodeList(const FBSceneNode* node)
{
    const flatbuffers::Vector<flatbuffers::Offset<FBSceneNode>>* out = nullptr;

    const char* nodeName = EnumNameFBGenericSceneNode(node->NodeInstance_type());
    Logger("[SceneLoader] Node name : %s", nodeName);

    switch (node->NodeInstance_type())
    {
    case FBGenericSceneNode_FBPCSoldier:
        out = (*(FBPCSoldier*)node->NodeInstance()).SceneNodes();
        break;

    case FBGenericSceneNode_FBCar:
        out = (*(FBPCSoldier*)node->NodeInstance()).SceneNodes();
        break;

    default:
        break;
    }

    return out;
}