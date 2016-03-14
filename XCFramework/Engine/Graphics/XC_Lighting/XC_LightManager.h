/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/XC_Lighting/LightingTypes.h"
#include "Engine/Graphics/XC_Camera/XC_CameraManager.h"
#include "Engine/Graphics/XC_GraphicsDx11.h"

class XC_LightManager
{
public:
    XC_LightManager(XC_CameraManager& cameraMgr);
    ~XC_LightManager();

    void                            InitializeLights();
    void                            Update(float dt);
    void                            Draw(XC_Graphics& graphicsSystem);
    void                            Destroy();

private:

    std::map<ELightType, ILight*>   m_Lights;
    XCVec3                          m_eyePos;

    XC_CameraManager&               m_cameraManagerSystem;
    D3DConstantBuffer*              m_pCBLightsPerFrame;
};
