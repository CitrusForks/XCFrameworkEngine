#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameStates/EditorStates/EditorRunningState.h"
#include "Gameplay/WorldEditor.h"

#include "Engine/Graphics/XCCamera/XCCameraManager.h"

using namespace GameState;

EditorRunningState::EditorRunningState(void)
{
    m_isPaused = false;
}

EditorRunningState::~EditorRunningState(void)
{
}

void EditorRunningState::Init(GameFiniteStateMachine& gameFSM)
{
    TaskManager& taskManger = (TaskManager&)SystemLocator::GetInstance()->RequestSystem("TaskManager");
    SystemContainer& sysContainer = (SystemContainer&)SystemLocator::GetInstance()->GetSystemContainer();
    XCCameraManager& cameraMgr = (XCCameraManager&)SystemLocator::GetInstance()->RequestSystem("CameraSystem");

    sysContainer.RegisterSystem<WorldEditor>("WorldEditor");
    m_worldEditor = (WorldEditor*) &sysContainer.CreateNewSystem("WorldEditor");
    m_worldEditor->Init(taskManger);

    m_gameActorFactory = (GameActorsFactory*) &SystemLocator::GetInstance()->RequestSystem("GameActorsFactory");

    //Initialize the lights
    m_lightManagerSystem = new XC_LightManager(cameraMgr);
    m_lightManagerSystem->InitializeLights();

    //Set the camera to FPS mode
    cameraMgr.SetCameraType(CAMERATYPE_FPS);

    //Add all the objects into the world
    SimpleSkyBox* pSkyBox = (SimpleSkyBox*) m_gameActorFactory->CreateActor("SimpleSkyBox");
    pSkyBox->PreLoad(toXMVECTOR(0.0f, 0.0f, 0.0f, 1.0f), toXMVECTOR(0.0f, 0.0f, 0.0f, 1.0f), toXMVECTOR(1000.0f, 1000.0f, 1000.0f, 1.0f), BasicMaterial(), "cubemap_bright", RASTERIZERTYPE_FILL_SOLID);
    m_worldEditor->RequestAddActor(pSkyBox);

    Terrain* pHeightMapTerrain = (Terrain*) m_gameActorFactory->CreateActor("Terrain");
    pHeightMapTerrain->PreLoad("Assets\\Textures\\Environment\\Terrain\\SimpleTerrain1.bmp",
                            "multi_grass",
                            "multi_ground",
                            "Sand",
                            "multi_blend",
                            XCVec3(50, -20, -40), 25, 25, 1.0f, 1.0f);
    m_worldEditor->RequestAddActor(pHeightMapTerrain);
}

void EditorRunningState::Update(float dt)
{
    m_lightManagerSystem->Update(dt);
    m_worldEditor->Update(dt);
}

void EditorRunningState::Draw(XC_Graphics& graphicsSystem)
{
    m_lightManagerSystem->Draw(graphicsSystem);
    m_worldEditor->Draw(graphicsSystem);
}

void EditorRunningState::Destroy()
{
    m_lightManagerSystem->Destroy();
    m_worldEditor->Destroy();
}