/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "XC_LightManager.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderBufferConstants.h"


XC_LightManager::XC_LightManager(XC_CameraManager& cameraMgr)
    : m_cameraManagerSystem(cameraMgr)
{
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_pCBLightsPerFrame = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(cbLightsPerFrame)));
}


XC_LightManager::~XC_LightManager()
{
#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    heap.DestroyBuffer(m_pCBLightsPerFrame);
#endif
}

void XC_LightManager::InitializeLights()
{
    //Add Lights - To remove. Need a LightManager- Manages all types of light, allows to add and remove lights from world. Maybe we can add this light actors directly into the world
    //Directional light
    DirectionalLight* directionalLight = new DirectionalLight();

    directionalLight->Ambient = XCVec4Unaligned(1.0f, 1.0f, 1.0f, 1.0f);
    directionalLight->Diffuse = XCVec4Unaligned(0.2f, 0.2f, 0.2f, 1.0f);
    directionalLight->Specular = XCVec4Unaligned(0.2f, 0.2f, 0.2f, 1.0f);
    directionalLight->Direction = XCVec3Unaligned(0.577f, -0.577f, 0.577f);

    m_Lights[LIGHTTYPE_DIRECTIONAL] = (ILight*)directionalLight;

    //Point Light
    PointLight* pointLight = new PointLight();
    pointLight->Ambient = XCVec4Unaligned(0.3f, 0.3f, 0.3f, 1.0f);
    pointLight->Diffuse = XCVec4Unaligned(0.7f, 0.7f, 0.7f, 1.0f);
    pointLight->Specular = XCVec4Unaligned(0.7f, 0.7f, 0.7f, 1.0f);
    pointLight->Att = XCVec3Unaligned(0.0f, 0.9f, 0.0f);
    pointLight->Range = 10000.0f;
    m_Lights[LIGHTTYPE_POINT] = (ILight*)pointLight;

    //Spot Light
    SpotLight* spotLight = new SpotLight();
    spotLight->Ambient = XCVec4Unaligned(0.0f, 0.0f, 0.0f, 1.0f);
    spotLight->Diffuse = XCVec4Unaligned(1.0f, 1.0f, 0.0f, 1.0f);
    spotLight->Specular = XCVec4Unaligned(1.0f, 1.0f, 1.0f, 1.0f);
    spotLight->Att = XCVec3Unaligned(1.0f, 0.0f, 0.0f);
    spotLight->Spot = 96.0f;
    spotLight->Range = 10000.0f;
    m_Lights[LIGHTTYPE_SPOT] = (ILight*)spotLight;

    m_eyePos = XCVec3(0, 0, 0);
}

void XC_LightManager::Update(float dt)
{
    // Build the view matrix.
    XCVecIntrinsic4 camPos = m_cameraManagerSystem.GetCameraPosition();
    m_eyePos = XCVec3(XMVectorGetX(camPos), XMVectorGetY(camPos), XMVectorGetZ(camPos));

    XCVecIntrinsic4 pos = XMVectorSet(XMVectorGetX(camPos), XMVectorGetY(camPos), XMVectorGetZ(camPos), 1.0f);
    XCVecIntrinsic4 target = m_cameraManagerSystem.GetCameraTarget();
    XCVecIntrinsic4 up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    // Circle light over the land surface.
    PointLight* pointLight = (PointLight*)m_Lights[LIGHTTYPE_POINT];
    pointLight->Position = XCVec3Unaligned(70.0f*cosf(0.2f), 10.0f, 70.0f*sinf(0.2f));

    // The spotlight takes on the camera position and is aimed in the
    // same direction the camera is looking. In this way, it looks
    // like we are holding a flashlight.
    SpotLight* spotLight = (SpotLight*)m_Lights[LIGHTTYPE_SPOT];
    spotLight->Position = ToXCVec3Unaligned(m_eyePos);

    DirectionalLight* directionalLight = (DirectionalLight*)m_Lights[LIGHTTYPE_DIRECTIONAL];
    XMStoreFloat3(&spotLight->Direction, XMVector3Normalize(target - pos));
    XMStoreFloat3(&directionalLight->Direction, XMVector3Normalize(target - pos));
}

void XC_LightManager::Draw(XC_Graphics& graphicsSystem)
{
    cbLightsPerFrame lightsPerFrame = { 
                                        *(DirectionalLight*)m_Lights[LIGHTTYPE_DIRECTIONAL], 
                                        *(PointLight*)m_Lights[LIGHTTYPE_POINT], 
                                        *(SpotLight*)m_Lights[LIGHTTYPE_SPOT], 
                                        ToXCVec3Unaligned(m_eyePos), 
                                        0 
                                      };

    memcpy(m_pCBLightsPerFrame->m_cbDataBegin, &lightsPerFrame, sizeof(cbLightsPerFrame));
    
    XCShaderHandle* shaderRef = nullptr;

    RenderingPool::RenderWorker* renderWorkers = graphicsSystem.GetRenderingPool().GetRenderWorkers();
    for (unsigned int workerIndex = 0; workerIndex < RenderingPool::NbRenderWorkerThreads; ++workerIndex)
    {
        RenderContext& context = renderWorkers[workerIndex].m_renderContext;
     
        context.ApplyShader(SHADERTYPE_SKINNEDCHARACTER);
        shaderRef = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(SHADERTYPE_SKINNEDCHARACTER);
        shaderRef->setConstantBuffer("cbLightsPerFrame", context.GetDeviceContext(), m_pCBLightsPerFrame->m_gpuHandle);

        context.ApplyShader(SHADERTYPE_LIGHTTEXTURE);
        shaderRef = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(SHADERTYPE_LIGHTTEXTURE);
        shaderRef->setConstantBuffer("cbLightsPerFrame", context.GetDeviceContext(), m_pCBLightsPerFrame->m_gpuHandle);

        context.ApplyShader(SHADERTYPE_TERRIANMULTITEXTURE);
        shaderRef = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(SHADERTYPE_TERRIANMULTITEXTURE);
        shaderRef->setConstantBuffer("cbLightsPerFrame", context.GetDeviceContext(), m_pCBLightsPerFrame->m_gpuHandle);
    }
}

void XC_LightManager::Destroy()
{
    for (int index = 0; index < LIGHTTYPE_MAX; index++)
    {
        delete(m_Lights[(ELightType)index]);
    }
    m_Lights.clear();
}