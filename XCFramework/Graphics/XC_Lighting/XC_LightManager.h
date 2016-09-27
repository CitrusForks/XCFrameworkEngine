/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/System/ISystem.h"

#include "Graphics/XC_Lighting/LightingTypes.h"
#include "Graphics/XC_Graphics.h"
#include "Graphics/GPUResource.h"

class XC_LightManager : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(XC_LightManager)

    XC_LightManager();
    ~XC_LightManager();

    void                            InitializeLights();
    void                            Update(f32 dt);
    void                            Draw(XC_Graphics& graphicsSystem);
    void                            Destroy();

    GPUResource&                    GetLightConstantBuffer() { return *m_pCBLightsPerFrame; }

protected:
    static const u32                NoOfLights = 5;

private:

    std::vector<LightSource*>       m_Lights;
    XCVec4                          m_eyePos;

    GPUResource*                    m_pCBLightsPerFrame;
};
