/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/BasicGeometry/TexturedPlane.h"
#include "Engine/Graphics/XC_Textures/Texture2D.h"

class LiveDriveTexturePlane : public TexturedPlane
{
public:
    DECLARE_OBJECT_CREATION(LiveDriveTexturePlane)

    LiveDriveTexturePlane();
    LiveDriveTexturePlane(XCVecIntrinsic4 p1, XCVecIntrinsic4 p2, XCVecIntrinsic4 p3);
    virtual ~LiveDriveTexturePlane();

    virtual void                     PreLoad(const void* fbBuffer);
    virtual void                     Update(float dt);
    virtual void                     Draw(RenderContext& renderContext);
    virtual void                     Destroy();

private:
    Texture2D*                       m_liveDriveTexture;
};