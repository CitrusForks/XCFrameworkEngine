/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "XC_CameraManager.h"

#include "Gameplay/XC_Camera/BasicCamera.h"
#include "Gameplay/XC_Camera/FirstPersonCamera.h"
#include "Gameplay/XC_Camera/ThirdPersonCamera.h"
#include "Gameplay/GameActors/TPCChaseableActor.h"

XC_CameraManager::XC_CameraManager()
{
}

XC_CameraManager::~XC_CameraManager()
{
}

void XC_CameraManager::InitializeCameras(XC_Graphics& graphicsSystem, int clientWidth, int clientHeight)
{
    m_graphicsSystem = &graphicsSystem;

    //Basic Camera
    m_Cameras[CAMERATYPE_BASIC] = std::make_unique<BasicCamera>(
                                                    toXMVECTOR(3, 5, -2.0f, 1.0f), //Position
                                                    toXMVECTOR(0, 0, 0, 0), //Target
                                                    toXMVECTOR(0, 1, 0, 0), //Up
                                                    (float)clientWidth / clientHeight, //Aspect Ratio
                                                    XM_PIDIV2, //FOV
                                                    0.01f, //Near plane
                                                    5000.0f //Far plane
                                                 );
    
    //FPS Camera
    m_Cameras[CAMERATYPE_FPS] = std::make_unique<FirstPersonCamera>(
                                                    toXMVECTOR(3, 2, -2.0f, 1.0f), //Position
                                                    toXMVECTOR(0, 0, 1, 0), //Target
                                                    toXMVECTOR(0, 1, 0, 0), //Up
                                                    (float)clientWidth / clientHeight, //Aspect Ratio
                                                    XM_PIDIV2, //FOV
                                                    0.01f, //Near plane
                                                    5000.0f //Far plane
                                                    );

    //TPS Camera
    m_Cameras[CAMERATYPE_TPS] = std::make_unique<ThirdPersonCamera>(
                                                    nullptr,
                                                    toXMVECTOR(3, 5, -2.0f, 1.0f), //Position
                                                    toXMVECTOR(0, 0, 1, 0), //Target
                                                    toXMVECTOR(0, 1, 0, 0), //Up
                                                    (float)clientWidth / clientHeight, //Aspect Ratio
                                                    XM_PIDIV2, //FOV
                                                    0.01f, //Near plane
                                                    5000.0f //Far plane
                                                    );

    //Set default camera
    m_currentCameraType = CAMERATYPE_BASIC;
}

void XC_CameraManager::AttachCameraToActor(IActor* actor)
{
    ((ThirdPersonCamera*) m_Cameras[CAMERATYPE_TPS].get())->AttachTo(actor);

    //Check if it's a chaseable actor, then get the actor and set the respective attributes
    TPCChaseableActor* tpcActor = dynamic_cast<TPCChaseableActor*> (actor);
    
    if (tpcActor != nullptr)
    {
        ((ThirdPersonCamera*) m_Cameras[CAMERATYPE_TPS].get())->SetDistanceFromTarget(tpcActor->getDistanceFromTarget());
    }
    else
    {
        AttachCameraToActor(actor, XMVectorZero());
    }
}

void XC_CameraManager::AttachCameraToActor(IActor* actor, XCVecIntrinsic4 distanceFromTarget)
{
    ((ThirdPersonCamera*) m_Cameras[CAMERATYPE_TPS].get())->AttachTo(actor);
    ((ThirdPersonCamera*) m_Cameras[CAMERATYPE_TPS].get())->SetDistanceFromTarget(distanceFromTarget);
}


void XC_CameraManager::Update(float dt)
{
    m_Cameras[m_currentCameraType]->Update(dt);

    //Set the global camera with the current camera values.
    XC_ShaderManager& shaderSystem = m_graphicsSystem->GetShaderManagerSystem();
    shaderSystem.GetGlobalShaderData().m_camera.SetProjectionMatrix(GetProjMatrix());
    shaderSystem.GetGlobalShaderData().m_camera.SetViewMatrix(GetViewMatrix());
    shaderSystem.GetGlobalShaderData().m_camera.SetPosition(GetCameraPosition());
    shaderSystem.GetGlobalShaderData().m_camera.SetTarget(GetCameraTarget());
}

void XC_CameraManager::Draw()
{
}

void XC_CameraManager::onResize(int clientWidth, int clientHeight)
{
    for (int index = 0; index < CAMERATYPE_MAX; index++)
    {
        if (m_Cameras[(ECameraType)index])
        {
            m_Cameras[(ECameraType)index]->OnResize(clientWidth, clientHeight);
        }
    }
}

void XC_CameraManager::Destroy()
{
    m_Cameras.clear();
}