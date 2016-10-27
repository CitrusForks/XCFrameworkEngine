/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "LiveDriveTexturePlane.h"

#include "Assets/Packages/Schema/BasicTypes_generated.h"
#include "Assets/Packages/Schema/GameplayActors_generated.h"

#include "Graphics/XCGraphics.h"
#include "Graphics/XCTextures/Texture2D.h"

#include "Engine/Resource/ResourceHandle.h"

LiveDriveTexturePlane::LiveDriveTexturePlane()
    : m_liveDriveTexture(nullptr)
{
}

LiveDriveTexturePlane::LiveDriveTexturePlane(XCVec4& p1, XCVec4& p2, XCVec4& p3)
    : TexturedPlane(p1, p2, p3)
{
}

LiveDriveTexturePlane::~LiveDriveTexturePlane()
{
}

void LiveDriveTexturePlane::PreLoad(const void* fbBuffer)
{
    const FBLiveDriveTexturePlane* texPlaneBuff = (FBLiveDriveTexturePlane*)fbBuffer;

    TexturedPlane::PreLoad(texPlaneBuff->Base());

    XCGraphics& graphicsSystem = (XCGraphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
    m_liveDriveTexture = XCNEW(Texture2D)(graphicsSystem.GetRenderTexture(RenderTargetType_LiveDrive).GetRenderTargetResource());
    m_texture->m_Resource = m_liveDriveTexture;
}

void LiveDriveTexturePlane::Update(f32 dt)
{
}

void LiveDriveTexturePlane::Draw(RenderContext& renderContext)
{
    XCGraphics& graphicsSystem = (XCGraphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
    if (!graphicsSystem.IsSecondaryDrawCall())
    {
        TexturedPlane::Draw(renderContext);
    }
}

void LiveDriveTexturePlane::Destroy()
{
    TexturedPlane::Destroy();

    if (m_liveDriveTexture)
    {
        m_liveDriveTexture->Destroy();
        XCDELETE(m_liveDriveTexture);
    }
}