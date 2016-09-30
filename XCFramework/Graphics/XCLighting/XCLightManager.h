/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/System/ISystem.h"

#include "Graphics/XCLighting/LightingTypes.h"
#include "Graphics/XCGraphics.h"
#include "Graphics/GPUResource.h"

class XCLightManager : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(XCLightManager)

    XCLightManager();
    ~XCLightManager();

    void                            InitializeLights();
    void                            Update(f32 dt);
    void                            Draw(XCGraphics& graphicsSystem);
    void                            Destroy();

    GPUResource&                    GetLightConstantBuffer() { return *m_pCBLightsPerFrame; }

protected:
    static const u32                NoOfLights = 5;

private:

    std::vector<LightSource*>       m_Lights;
    XCVec4                          m_eyePos;

    GPUResource*                    m_pCBLightsPerFrame;
};
