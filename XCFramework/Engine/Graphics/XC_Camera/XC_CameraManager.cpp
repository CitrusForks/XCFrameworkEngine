/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "XC_CameraManager.h"
#include "Engine/Graphics/XC_Camera/BasicCamera.h"
#include "Engine/Graphics/XC_Camera/FirstPersonCamera.h"

#if !defined(LOAD_SHADERS_FROM_DATA)
#include "Engine/Graphics/XC_Shaders/src/SolidColor.h"
#include "Engine/Graphics/XC_Shaders/src/LightTexture.h"
#include "Engine/Graphics/XC_Shaders/src/TerrainMultiTex.h"
#include "Engine/Graphics/XC_Shaders/src/CubeMap.h"
#endif

#include "Engine/Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Engine/Graphics/XC_Camera/ThirdPersonCamera.h"
#include "Gameplay/GameActors/TPCChaseableActor.h"
#include "Engine/Graphics/RenderingPool.h"


XC_CameraManager::XC_CameraManager()
{
}

XC_CameraManager::~XC_CameraManager()
{
}

void XC_CameraManager::InitializeCameras(XC_Graphics& graphicsSystem, DirectInput& directinput, int clientWidth, int clientHeight)
{
    m_graphicsSystem = &graphicsSystem;
    m_directInput = &directinput;

    //Basic Camera
    m_Cameras[CAMERATYPE_BASIC] = std::make_unique<BasicCamera>(
                                                    *m_directInput,
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
                                                    *m_directInput,
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
                                                    *m_directInput,
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
}

void XC_CameraManager::Draw()
{
/*
#if defined(XC_ORBIS1)
    XCMatrix4 view = XMMatrixLookAtLH(XCVecIntrinsic4(0, 50, 100, 0), XCVecIntrinsic4(0, 0, 0, 0), XCVecIntrinsic4(0, 1, 0, 0));
    XCMatrix4 proj = XMMatrixPerspectiveFovLH(55.0f * M_PI/180.0f, 16.0f / 9.0f, 1.0f, 1000.0f);

    cbView  vbuffer = { ToXCMatrix4Unaligned(XMMatrixTranspose(view)) };
    cbProj  pbuffer = { ToXCMatrix4Unaligned(XMMatrixTranspose(proj)) };
#else
    cbView  vbuffer = { ToXCMatrix4Unaligned(XMMatrixTranspose(m_Cameras[m_currentCameraType]->GetViewMatrix())) };
    cbProj  pbuffer = { ToXCMatrix4Unaligned(XMMatrixTranspose(m_Cameras[m_currentCameraType]->GetProjectionMatrix())) };
#endif

    //Get all rendercontext
    RenderingPool::RenderWorker* renderWorkers = m_graphicsSystem->GetRenderingPool().GetRenderWorkers();
    if(renderWorkers)
    {
        for (unsigned int workerIndex = 0; workerIndex < RenderingPool::NbRenderWorkerThreads; ++workerIndex)
        {
            RenderContext& context = renderWorkers[workerIndex].m_renderContext;

            context.ApplyShader(ShaderType_SolidColor);
            SolidColorShader* solidColorShader = (SolidColorShader*)context.GetShaderManagerSystem().GetShader(ShaderType_SolidColor);
            solidColorShader->setCBView(context.GetDeviceContext(), vbuffer);
            solidColorShader->setCBProj(context.GetDeviceContext(), pbuffer);

            context.ApplyShader(ShaderType_LightTexture);
            LightTextureShader* lightTexShader = (LightTextureShader*)context.GetShaderManagerSystem().GetShader(ShaderType_LightTexture);
            lightTexShader->setCBView(context.GetDeviceContext(), vbuffer);
            lightTexShader->setCBProj(context.GetDeviceContext(), pbuffer);

            context.ApplyShader(ShaderType_TerrainMultiTexture);
            TerrainMultiTex* terrainTexShader = (TerrainMultiTex*)context.GetShaderManagerSystem().GetShader(ShaderType_TerrainMultiTexture);
            terrainTexShader->setCBView(context.GetDeviceContext(), vbuffer);
            terrainTexShader->setCBProj(context.GetDeviceContext(), pbuffer);
        }
    }*/
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