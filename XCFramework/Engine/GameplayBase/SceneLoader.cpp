/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "SceneLoader.h"
#include "SceneGraph.h"
#include "Engine/Serializer/ActorSerializer.h"
#include "Assets/Packages/PackageConsts.h"

SceneLoader::SceneLoader(SceneGraph& world, std::string packageData)
    : AsyncTask("SceneLoader")
    , m_packageName(packageData)
    , m_sceneRef(world)
    , m_loadRequested(false)
{
}

SceneLoader::~SceneLoader()
{
}

void SceneLoader::Init()
{
    AsyncTask::Init();
}

void SceneLoader::Run()
{
    AsyncTask::Run();

    if (!m_loadRequested)
    {
        FlatBuffersSystem& fbSystem = SystemLocator::GetInstance()->RequestSystem<FlatBuffersSystem>("FlatBuffersSystem");
        fbSystem.ParseAndLoadFile(SCENE_SCHEMA_FILEPATH, m_sceneRef.GetMutableFlatBuffer());
        fbSystem.ParseAndLoadFile(m_packageName, m_sceneRef.GetMutableFlatBuffer());

        auto fbSceneGraph = GetFBSceneGraph(m_sceneRef.GetMutableFlatBuffer().GetBufferFromLoadedData());
        ActorSerializer::LoadNodeList<FBSceneNode>(m_sceneRef.GetSceneGraphTree().GetRootNode(), fbSceneGraph->SceneNodes());
        m_loadRequested = true;
    }

    Destroy();
}
