/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Engine/Resource/ResourceManager.h"
#include "Engine/GameplayBase/SceneGraph.h"

#include "Gameplay/GameStates/RunningState.h"

#ifdef LEGACY_LOADING
#include "Gameplay/GameActors/GameActorsFactory.h"
#endif

#include "Physics/PhysicsPlayground.h"

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

    m_cameraSystem = &SystemLocator::GetInstance()->RequestSystem<XCCameraManager>("CameraManager");
    m_directInput = &SystemLocator::GetInstance()->RequestSystem<XCInput>("InputSystem");
    m_worldSystem = &SystemLocator::GetInstance()->RequestSystem<SceneGraph>("SceneGraph");
    m_physicsPlayground = &SystemLocator::GetInstance()->RequestSystem<PhysicsPlayground>("PhysicsPlayground");
    
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    XCGraphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XCGraphics>("GraphicsSystem");

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
    pSkyBox->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pSkyBox));

    std::unique_ptr<TexturedPlane> pTexturedPlane = (std::unique_ptr<TexturedPlane>)(TexturedPlane*)actorFactory.CreateActor("TexturedPlane");
    pTexturedPlane->PreLoad(toXMVECTOR(0.0f, 5.0f, 10.0f, 1.0f), toXMVECTOR(0.0f, 0.0f, 0.0f, 1.0f), toXMVECTOR(5.0f, 5.0f, 0.0f, 1.0f), Material(), graphicsSystem.GetRenderTexture(RenderTargetType_LiveDrive), RasterType_FillSolid);
    m_worldSystem->RequestAddActor(std::move(pTexturedPlane));

    std::unique_ptr<Waves> pWave = (std::unique_ptr<Waves>)(Waves*)actorFactory.CreateActor("Waves");
    pWave->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pWave));
    
    std::unique_ptr<Terrain> pHeightMapTerrain = (std::unique_ptr<Terrain>)(Terrain*)actorFactory.CreateActor("Terrain");
    pHeightMapTerrain->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pHeightMapTerrain));
    
    pHeightMapTerrain = (std::unique_ptr<Terrain>)(Terrain*)actorFactory.CreateActor("Terrain");
    pHeightMapTerrain->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pHeightMapTerrain));


#if LOAD_STRESS_ENABLE
    pHeightMapTerrain = (std::unique_ptr<Terrain>)(Terrain*)actorFactory.CreateActor("Terrain");
    pHeightMapTerrain->LoadMetaData();
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
    pCarMesh->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pCarMesh));

    std::unique_ptr<NPCCar> pCarMeshNPC = (std::unique_ptr<NPCCar>)(NPCCar*)actorFactory.CreateActor("NPCCar");
    pCarMeshNPC->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pCarMeshNPC));

    std::unique_ptr<Door> pDoorMesh = (std::unique_ptr<Door>)(Door*)actorFactory.CreateActor("Door");
    pDoorMesh->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pDoorMesh));

    std::unique_ptr<PCSoldier> pSoldierMesh = (std::unique_ptr<PCSoldier>)(PCSoldier*)actorFactory.CreateActor("PCSoldier");
    pSoldierMesh->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pSoldierMesh));

    std::unique_ptr<NPCSoldier> pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    
#if LOAD_STRESS_ENABLE
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->LoadMetaData();
    m_worldSystem->RequestAddActor(std::move(pSoldierMeshNPC));
    pSoldierMeshNPC = (std::unique_ptr<NPCSoldier>)(NPCSoldier*)actorFactory.CreateActor("NPCSoldier");
    pSoldierMeshNPC->LoadMetaData();
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
            //m_worldSystem->EnablePhysics(true);
        }

        if (m_directInput->KeyDown(INPUT_KEY_6))
        {
            //m_worldSystem->EnablePhysics(false);
        }
    }

    m_physicsPlayground->Update(dt);
    m_worldSystem->Update(dt);

    if (m_directInput->KeyDown(INPUT_KEY_ESCAPE))
    {
        //Goto end state
        Event_GameStateChange event("EndState", STATE_DESTROY);
        EventBroadcaster& broadcaster = (EventBroadcaster&)SystemLocator::GetInstance()->RequestSystem("EventBroadcaster");
        broadcaster.BroadcastEvent(&event);
    }
}


void RunningState::Draw(XCGraphics& graphicsSystem)
{
    if (m_worldSystem->IsWorldReady())
    {
        m_worldSystem->Draw(graphicsSystem);
    }
}

void RunningState::Destroy()
{
    m_worldSystem->SetSceneReady(false);
    m_worldSystem->Destroy();
}