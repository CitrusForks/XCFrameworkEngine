/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "XC_CameraManager.h"

#include "Engine/GameplayBase/Camera/ICamera.h"

#include "Graphics/XC_Shaders/XC_ShaderContainer.h"

#include "Gameplay/GameActors/IActor.h"
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

void XC_CameraManager::Init(XC_Graphics& graphicsSystem, i32 clientWidth, i32 clientHeight)
{
    m_graphicsSystem = &graphicsSystem;

    //Basic Camera
    m_Cameras[CAMERATYPE_BASIC] = std::make_unique<BasicCamera>(
        XCVec4(3, 5, -2.0f, 1.0f), //Position
        XCVec4(0, 0, 0, 0), //Target
        XCVec4(0, 1, 0, 0), //Up
        (f32)clientWidth / clientHeight, //Aspect Ratio
        XC_PIDIV2, //FOV
        0.01f, //Near plane
        5000.0f //Far plane
        );
    
    //FPS Camera
    m_Cameras[CAMERATYPE_FPS] = std::make_unique<FirstPersonCamera>(
        XCVec4(3, 2, -2.0f, 1.0f), //Position
        XCVec4(0, 0, 1, 0), //Target
        XCVec4(0, 1, 0, 0), //Up
        (f32)clientWidth / clientHeight, //Aspect Ratio
        XC_PIDIV2, //FOV
        0.01f, //Near plane
        5000.0f //Far plane
        );

    //TPS Camera
    m_Cameras[CAMERATYPE_TPS] = std::make_unique<ThirdPersonCamera>(
        nullptr,
        XCVec4(3, 5, -2.0f, 1.0f), //Position
        XCVec4(0, 0, 1, 0), //Target
        XCVec4(0, 1, 0, 0), //Up
        (f32)clientWidth / clientHeight, //Aspect Ratio
        XC_PIDIV2, //FOV
        0.01f, //Near plane
        5000.0f //Far plane
        );

    //Set default camera
    m_currentCameraType = CAMERATYPE_BASIC;

    ISystem::Init();
}

void XC_CameraManager::AttachCameraToActor(IActor* actor)
{
    ((ThirdPersonCamera*) m_Cameras[CAMERATYPE_TPS].get())->AttachTo(actor);

    //Check if it's a chaseable actor, then get the actor and set the respective attributes
    TPCChaseableActor* tpcActor = dynamic_cast<TPCChaseableActor*> (actor);
    
    if (tpcActor != nullptr)
    {
        ((ThirdPersonCamera*) m_Cameras[CAMERATYPE_TPS].get())->SetDistanceFromTarget(tpcActor->GetDistanceFromTarget());
    }
    else
    {
        AttachCameraToActor(actor, XCVec4());
    }
}

void XC_CameraManager::AttachCameraToActor(IActor* actor, XCVec4& distanceFromTarget)
{
    ((ThirdPersonCamera*) m_Cameras[CAMERATYPE_TPS].get())->AttachTo(actor);
    ((ThirdPersonCamera*) m_Cameras[CAMERATYPE_TPS].get())->SetDistanceFromTarget(distanceFromTarget);
}

void XC_CameraManager::Update(f32 dt)
{
    m_Cameras[m_currentCameraType]->Update(dt);

    //Set the global camera with the current camera values.
    XC_ShaderContainer& shaderSystem = m_graphicsSystem->GetShaderContainer();
    shaderSystem.GetGlobalShaderData().m_camera.SetProjectionMatrix(GetProjMatrix());
    shaderSystem.GetGlobalShaderData().m_camera.SetViewMatrix(GetViewMatrix());
    shaderSystem.GetGlobalShaderData().m_camera.SetPosition(GetCameraPosition());
    shaderSystem.GetGlobalShaderData().m_camera.SetTarget(GetCameraTarget());
}

void XC_CameraManager::Draw()
{
}

void XC_CameraManager::onResize(i32 clientWidth, i32 clientHeight)
{
    for (i32 index = 0; index < CAMERATYPE_MAX; index++)
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