/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Engine/Resource/ResourceManager.h"

#include "Gameplay/GameStates/RunningState.h"

#ifdef LEGACY_LOADING
#include "Gameplay/GameActors/GameActorsFactory.h"
#endif

using namespace GameState;

RunningState::RunningState(void)
{
    m_isPaused	 = false;
    m_bMainPlayerSet = false;
}

RunningState::~RunningState(void)
{
    Logger("[GAME STATE RUNNING] DESTROYING RUNNING STATE");
}

void RunningState::Init()
{
    IGameState::Init();

    m_cameraSystem = &SystemLocator::GetInstance()->RequestSystem<XC_CameraManager>("CameraManager");
    m_directInput = &SystemLocator::GetInstance()->RequestSystem<DirectInput>("InputSystem");
    m_worldSystem = &SystemLocator::GetInstance()->RequestSystem<World>("World");

    ResourceManager& resMgr         = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    XC_Graphics& graphicsSystem     = SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");

    //Initialize the lights
    SystemContainer& container = (SystemContainer&)SystemLocator::GetInstance()->GetSystemContainer();
    container.RegisterSystem<XC_LightManager>("LightsManager");

    m_lightManagerSystem = (XC_LightManager*)&container.CreateNewSystem("LightsManager");
    m_lightManagerSystem->InitializeLights();

    //Set the camera to FPS mode
    m_cameraSystem->SetCameraType(CAMERATYPE_FPS);

#ifdef LEGACY_LOADING
    GameActorsFactory& actorFactory = SystemLocator::GetInstance()->RequestSystem<GameActorsFactory>("GameActorsFactory");

    //Add all the objects into the world
#if LOAD_STRESS_ENABLE
    std::unique_ptr<SimpleActor> pBox = (std::unique_ptr<SimpleActor>)(SimpleActor*)actorFactory.CreateActor("BoxGeometry");
    m_worldSystem->RequestAddActor(std::move(pBox));
#endif

    std::unique_ptr<SimpleSkyBox> pSkyBox = (std::unique_ptr<SimpleSkyBox>)(SimpleSkyBox*)actorFactory.CreateActor("SimpleSkyBox");
    pSkyBox->PreLoad(toXMVECTOR(0.0f, 0.0f, 0.0f, 1.0f), toXMVECTOR(0.0f, 0.0f, 0.0f, 1.0f), toXMVECTOR(1000.0f, 1000.0f, 1000.0f, 1.0f), Material(), (CubeTexture3D*)resMgr.AcquireResource("cubemap_bright"), RasterType_FillSolid);
    m_worldSystem->RequestAddActor(std::move(pSkyBox));

    std::unique_ptr<TexturedPlane> pTexturedPlane = (std::unique_ptr<TexturedPlane>)(TexturedPlane*)actorFactory.CreateActor("TexturedPlane");
    pTexturedPlane->PreLoad(toXMVECTOR(0.0f, 5.0f, 10.0f, 1.0f), toXMVECTOR(0.0f, 0.0f, 0.0f, 1.0f), toXMVECTOR(5.0f, 5.0f, 0.0f, 1.0f), Material(), graphicsSystem.GetRenderTexture(RENDERTARGET_LIVEDRIVE), RasterType_FillSolid);
    m_worldSystem->RequestAddActor(std::move(pTexturedPlane));

    std::unique_ptr<Waves> pWave = (std::unique_ptr<Waves>)(Waves*)actorFactory.CreateActor("Waves");
    pWave->PreLoad(XCVec3(100, 0, 0), 50, 50, 1, 1);
    m_worldSystem->RequestAddActor(std::move(pWave));
    
    std::unique_ptr<Terrain> pHeightMapTerrain = (std::unique_ptr<Terrain>)(Terrain*)actorFactory.CreateActor("Terrain");
    pHeightMapTerrain->PreLoad("Assets\\Textures\\Environment\\Terrain\\SimpleTerrain1.bmp",
                                                    (Texture2D*)resMgr.AcquireResource("multi_grass"),
                                                    (Texture2D*)resMgr.AcquireResource("multi_ground"), 
                                                    (Texture2D*)resMgr.AcquireResource("Sand"),
                                                    (Texture2D*)resMgr.AcquireResource("multi_blend"),
                                                    XCVec3(50, -20, -40), 25, 25, 1.0f, 1.0f);
    m_worldSystem->RequestAddActor(std::move(pHeightMapTerrain));
    
    pHeightMapTerrain = (std::unique_ptr<Terrain>)(Terrain*)actorFactory.CreateActor("Terrain");
    pHeightMapTerrain->PreLoad("Assets\\Textures\\Environment\\Terrain\\terrain1.bmp",
                                                  (Texture2D*)resMgr.AcquireResource("Sand"),
                                                  (Texture2D*)resMgr.AcquireResource("multi_ground"),
                                                  (Texture2D*)resMgr.AcquireResource("multi_grass"),
                                                  (Texture2D*)resMgr.AcquireResource("multi_blend"),
                                                  XCVec3(300, -20, -40), 25, 25, 1.0f, 1.0f);
    m_worldSystem->RequestAddActor(std::move(pHeightMapTerrain));


#if LOAD_STRESS_ENABLE
    pHeightMapTerrain = (std::unique_ptr<Terrain>)(Terrain*)actorFactory.CreateActor("Terrain");
    pHeightMapTerrain->PreLoad("Assets\\Textures\\Environment\\Terrain\\GrayscaleImages\\gradient1.bmp",
                                                  (Texture2D*)resMgr.AcquireResource("Sand"),
                                                  (Texture2D*)resMgr.AcquireResource("multi_ground"),
                                                  (Texture2D*)resMgr.AcquireResource("multi_grass"),
                                                  (Texture2D*)resMgr.AcquireResource("multi_blend"),
                                                  XCVec3(50, -20, -300), 25, 25, 1.0f, 1.0f);
    m_worldSystem->RequestAddActor(std::move(pHeightMapTerrain));

    pHeightMapTerrain = std::make_unique<Terrain>("Assets\\Textures\\Environment\\Terrain\\GrayscaleImages\\gradient2.bmp",
                                                    (Texture2D*)resMgr.AcquireResource("Sand"),
                                                    (Texture2D*)resMgr.AcquireResource("multi_ground"),
                                                    (Texture2D*)resMgr.AcquireResource("multi_grass"),
                                                    (Texture2D*)resMgr.AcquireResource("multi_blend"),
                                                    XCVec3(300, -20, 100), 25, 25, 1.0, 1.0);

    pHeightMapTerrain = std::make_unique<Terrain>("Assets\\Textures\\Environment\\Terrain\\GrayscaleImages\\gradient.bmp",
    (Texture2D*)resMgr.AcquireResource("Sand"),
    (Texture2D*)resMgr.AcquireResource("multi_ground"),
    (Texture2D*)resMgr.AcquireResource("multi_grass"),
    (Texture2D*)resMgr.AcquireResource("multi_blend"),
    XCVec3(300, -20, -200), 25, 25, 1.0, 1.0);
#endif

    std::unique_ptr<PCCar> pCarMesh = (std::unique_ptr<PCCar>)(PCCar*)actorFactory.CreateActor("PCCar");
    pCarMesh->PreLoad(XCVec3(0, 2, 0), (XCMesh*)resMgr.AcquireResource("PorcheCar"));
    m_worldSystem->RequestAddActor(std::move(pCarMesh));

    std::unique_ptr<NPCCar> pCarMeshNPC = (std::unique_ptr<NPCCar>)(NPCCar*)actorFactory.CreateActor("NPCCar");
    pCarMeshNPC->PreLoad(XCVec3(10, 2, 10), (XCMesh*)resMgr.AcquireResource("PorcheCar"));
    m_worldSystem->RequestAddActor(std::move(pCarMeshNPC));

    std::unique_ptr<Door> pDoorMesh = (std::unique_ptr<Door>)(Door*)actorFactory.CreateActor("Door");
    pDoorMesh->PreLoad((Texture2D*)resMgr.AcquireResource("multi_ground"), XCVec3(0, 2, 10), (XCMesh*)resMgr.AcquireResource("WoodenDoor"));
    m_worldSystem->RequestAddActor(std::move(pDoorMesh));

    std::unique_ptr<PCSoldier> pSoldierMesh = (std::unique_ptr<PCSoldier>)(PCSoldier*)actorFactory.CreateActor("PCSoldier");
    pSoldierMesh->PreLoad(XCVec3(10, 2, 10), (XCMesh*)resMgr.AcquireResource("Soldier"));
    m_worldSystem->RequestAddActor(std::move(pSoldierMesh));

    std::unique_ptr<NPCSoldier> pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->PreLoad(XCVec3(-30, 2, 50), (XCMesh*)resMgr.AcquireResource("Soldier"));
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->PreLoad(XCVec3(-30, 2, 60), (XCMesh*)resMgr.AcquireResource("Soldier"));
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->PreLoad(XCVec3(-30, 2, 70), (XCMesh*)resMgr.AcquireResource("Soldier"));
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->PreLoad(XCVec3(-30, 2, 80), (XCMesh*)resMgr.AcquireResource("Soldier"));
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->PreLoad(XCVec3(-30, 2, 90), (XCMesh*)resMgr.AcquireResource("Soldier"));
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    
#if LOAD_STRESS_ENABLE
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->PreLoad(XCVec3(-30, 2, 10), (XCMesh*)resMgr.AcquireResource("Soldier"));
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->PreLoad(XCVec3(-30, 2, 20), (XCMesh*)resMgr.AcquireResource("Soldier"));
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->PreLoad(XCVec3(-30, 2, 30), (XCMesh*)resMgr.AcquireResource("Soldier"));
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->PreLoad(XCVec3(-30, 2, 40), (XCMesh*)resMgr.AcquireResource("Soldier"));
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->PreLoad(XCVec3(-30, 2, 100), (XCMesh*)resMgr.AcquireResource("Soldier"));
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
#endif
#endif
}

void RunningState::Update(f32 dt)
{
    //This is hack, the world should tell whether it's ready by checking all the actors are loaded correctly?
    if (m_worldSystem->IsWorldReady())
    {
        if (m_directInput->KeyDown(INPUT_KEY_1))
        {
            m_cameraSystem->SetCameraType(CAMERATYPE_FPS);
        }

        if (m_directInput->KeyDown(INPUT_KEY_2))
        {
            m_cameraSystem->SetCameraType(CAMERATYPE_TPS);
        }

        if (m_directInput->KeyDown(INPUT_KEY_3))
        {
            //Cycle through playable characters
            if (m_worldSystem->IsActorReady(m_worldSystem->GetMainPlayableCharacter()))
            {
                m_worldSystem->SetMainPlayableCharacter(m_worldSystem->GetNextPlayableActor(m_worldSystem->GetMainPlayableCharacter()));
                m_cameraSystem->AttachCameraToActor(m_worldSystem->GetActor(m_worldSystem->GetMainPlayableCharacter()));
            }
        }

        if (m_directInput->KeyDown(INPUT_KEY_5))
        {
            m_worldSystem->EnablePhysics(true);
        }

        if (m_directInput->KeyDown(INPUT_KEY_6))
        {
            m_worldSystem->EnablePhysics(false);
        }

        m_lightManagerSystem->Update(dt);
        m_worldSystem->Update(dt);
    }

    if (m_directInput->KeyDown(INPUT_KEY_ESCAPE))
    {
        //Goto end state
        Event_GameStateChange event("EndState", STATE_DESTROY);
        EventBroadcaster& broadcaster = (EventBroadcaster&)SystemLocator::GetInstance()->RequestSystem("EventBroadcaster");
        broadcaster.BroadcastEvent(&event);
    }
}


void RunningState::Draw(XC_Graphics& graphicsSystem)
{
    if (m_worldSystem->IsWorldReady())
    {
        m_lightManagerSystem->Draw(graphicsSystem);
        m_worldSystem->Draw(graphicsSystem);
    }
}

void RunningState::Destroy()
{
    m_lightManagerSystem->Destroy();

    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RemoveSystem("LightsManager");

    m_worldSystem->SetWorldReady(false);
    m_worldSystem->Destroy();
}