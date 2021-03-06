/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "XCLightManager.h"

#include "Graphics/GPUResourceSystem.h"
#include "Graphics/XCShaders/XCShaderHandle.h"
#include "Graphics/XCShaders/XCShaderBufferConstants.h"

XCLightManager::XCLightManager()
{
}

XCLightManager::~XCLightManager()
{
}

void XCLightManager::InitializeLights()
{
    GPUResourceSystem& gpuSys = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
    m_pCBLightsPerFrame = gpuSys.CreateConstantBufferResourceView(GPUResourceDesc(GPUResourceType_CBV, sizeof(cbLightsPerFrame)));

    //Add Lights - To remove. Need a LightManager- Manages all types of light, allows to add and remove lights from world. Maybe we can add this light actors directly into the world
    for (u32 index = 0; index < NoOfLights; ++index)
    {
        LightSource* lightSource = XCNEW(LightSource)();

        lightSource->LightColor  = XCVec4Unaligned(1.0f, 1.0f, 1.0f, 1.0f);
        lightSource->Intensity   = XCVec4Unaligned(10.0f, 10.0f, 10.0f, 1.0f);
        lightSource->Direction   = XCVec4Unaligned(0.0f, 1.0f, 0.0f, 0.0f);
        lightSource->Position    = XCVec4Unaligned(0.0f, 10.0f, index * 50.0f, 1.0f);

        m_Lights.push_back(lightSource);
    }

    m_eyePos = XCVec4(0, 0, 0, 0);
}

void XCLightManager::Update(f32 dt)
{
    // Build the view matrix.
    XCGraphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XCGraphics>("GraphicsSystem");
    XCShaderContainer& shaderSystem = graphicsSystem.GetShaderContainer();

    XCVec4 camPos = shaderSystem.GetGlobalShaderData().m_camera.GetPosition();

    m_eyePos       = camPos;
    
    XCVec4 target  = shaderSystem.GetGlobalShaderData().m_camera.GetTarget();
    XCVec4 up(0.0f, 1.0f, 0.0f, 0.0f);
    XCVec4 direction = VectorNormalize<3>(target - m_eyePos);
}

void XCLightManager::Draw(XCGraphics& graphicsSystem)
{
    cbLightsPerFrame lightsPerFrame = {};

    for (u32 index = 0; index < NoOfLights; ++index)
    {
        lightsPerFrame.gLightSource[index] = *m_Lights[index];
    }
    lightsPerFrame.gNoOfLights = XCVec4Unaligned(NoOfLights, NoOfLights, NoOfLights, 1.0f);

    m_pCBLightsPerFrame->UploadDataOnGPU(*graphicsSystem.GetDeviceContext(), &lightsPerFrame, sizeof(cbLightsPerFrame));
}

void XCLightManager::Destroy()
{
    for (i32 index = 0; index < NoOfLights; index++)
    {
        XCDELETE(m_Lights[index]);
    }
    m_Lights.clear();

    GPUResourceSystem& gpuSys = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
    gpuSys.DestroyResource(m_pCBLightsPerFrame);
}