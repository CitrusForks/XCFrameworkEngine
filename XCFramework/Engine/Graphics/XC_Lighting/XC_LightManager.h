/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/System/ISystem.h"
#include "Engine/Graphics/XC_Lighting/LightingTypes.h"
#include "Engine/Graphics/XC_Camera/XC_CameraManager.h"
#include "Engine/Graphics/XC_Graphics.h"

class XC_LightManager : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(XC_LightManager)

    XC_LightManager();
    ~XC_LightManager();

    void                            InitializeLights();
    void                            Update(float dt);
    void                            Draw(XC_Graphics& graphicsSystem);
    void                            Destroy();

    D3DConstantBuffer*              getLightConstantBuffer() { return m_pCBLightsPerFrame; }

private:

    std::map<ELightType, ILight*>   m_Lights;
    XCVec3                          m_eyePos;

    XC_CameraManager*               m_cameraManagerSystem;
    D3DConstantBuffer*              m_pCBLightsPerFrame;
};
