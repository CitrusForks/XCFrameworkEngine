/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "FontActor.h"

#include "Engine/Resource/ResourceManager.h"
#include "Graphics/XCMesh/VectorFontMesh.h"

FontActor::FontActor()
{
}

FontActor::~FontActor(void)
{
}

IActor::ActorReturnState FontActor::Init()
{
    IActor::ActorReturnState result = IActor::Init();

    m_transformedRotation = m_MRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    return result;
}

IActor::ActorReturnState FontActor::LoadMetaData( const void* metaData )
{
    const FBFont* font = (FBFont*)metaData;

    IActor::LoadMetaData(font->Base());

    ResourceManager& resMgr = (ResourceManager&)SystemLocator::GetInstance()->RequestSystem("ResourceManager");

    m_fontMesh = &resMgr.AcquireResource(font->FontResourceName()->c_str());

    return IActor::ActorReturnState_Success;
}

IActor::ActorReturnState FontActor::Load()
{
    if (m_fontMesh == nullptr || (m_fontMesh && m_fontMesh->GetResource<VectorFontMesh>()->IsLoaded()))
    {
        return IActor::Load();
    }

    return IActor::ActorReturnState_Processing;
}

IActor::ActorReturnState FontActor::Update(f32 dt)
{
    return IActor::Update(dt);
}

bool FontActor::Draw(RenderContext& renderContext)
{
    IActor::Draw(renderContext);

    m_fontMesh->GetResource<VectorFontMesh>()->DrawText("ABCD", XCVec3Unaligned(1.0f, 10.0f, 0.0f), renderContext);

    return true;
}

IActor::ActorReturnState FontActor::Destroy()
{
    return IActor::Destroy();
}
