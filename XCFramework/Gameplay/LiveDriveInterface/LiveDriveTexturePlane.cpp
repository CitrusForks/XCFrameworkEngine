/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "LiveDriveTexturePlane.h"

#include "Assets/Packages/Schema/BasicTypes_generated.h"
#include "Assets/Packages/Schema/GameplayActors_generated.h"

#include "Graphics/XC_Graphics.h"

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
    m_currentPosition.SetValues(texPlaneBuff->Position()->x(), texPlaneBuff->Position()->y(), texPlaneBuff->Position()->z(), texPlaneBuff->Position()->w());
    m_initialRotation.SetValues(texPlaneBuff->Rotation()->x(), texPlaneBuff->Rotation()->y(), texPlaneBuff->Rotation()->z(), texPlaneBuff->Rotation()->w());
    m_initialScaling.SetValues(texPlaneBuff->Scaling()->x(), texPlaneBuff->Scaling()->y(), texPlaneBuff->Scaling()->z(), texPlaneBuff->Scaling()->w());

    m_material.Ambient  = XCVec4(texPlaneBuff->Material()->Ambient()->x(), texPlaneBuff->Material()->Ambient()->y(), texPlaneBuff->Material()->Ambient()->z(), texPlaneBuff->Material()->Ambient()->w());
    m_material.Diffuse  = XCVec4(texPlaneBuff->Material()->Diffuse()->x(), texPlaneBuff->Material()->Diffuse()->y(), texPlaneBuff->Material()->Diffuse()->z(), texPlaneBuff->Material()->Diffuse()->w());
    m_material.Specular = XCVec4(texPlaneBuff->Material()->Specular()->x(), texPlaneBuff->Material()->Specular()->y(), texPlaneBuff->Material()->Specular()->z(), texPlaneBuff->Material()->Specular()->w());

    XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
    m_liveDriveTexture = XCNEW(Texture2D)(graphicsSystem.GetRenderTexture(RENDERTARGET_LIVEDRIVE).GetShaderResourceView());
    m_texture->m_Resource = m_liveDriveTexture;
}

void LiveDriveTexturePlane::Update(f32 dt)
{
}

void LiveDriveTexturePlane::Draw(RenderContext& renderContext)
{
    XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
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