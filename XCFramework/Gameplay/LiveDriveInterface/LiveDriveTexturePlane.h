/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/SimpleActor.h"

#include "Graphics/BasicGeometry/TexturedPlane.h"
#include "Graphics/XC_Textures/Texture2D.h"

class LiveDriveTexturePlane : public TexturedPlane
{
public:
    DECLARE_OBJECT_CREATION(LiveDriveTexturePlane)

    LiveDriveTexturePlane();
    LiveDriveTexturePlane(XCVec4& p1, XCVec4& p2, XCVec4& p3);
    virtual ~LiveDriveTexturePlane();

    virtual void                     PreLoad(const void* fbBuffer);
    virtual void                     Update(f32 dt);
    virtual void                     Draw(RenderContext& renderContext);
    virtual void                     Destroy();

private:
    Texture2D*                       m_liveDriveTexture;
};