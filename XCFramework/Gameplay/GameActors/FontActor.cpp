/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameActors/FontActor.h"
#include "Engine/Resource/ResourceManager.h"
#include "Graphics/XCMesh/VectorFontMesh.h"

FontActor::FontActor()
{
}

FontActor::~FontActor(void)
{
}

void FontActor::Init(i32 actorId)
{
    IActor::Init(actorId);

    m_transformedRotation = m_MRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;
}

void FontActor::PreLoad(const void* fbBuffer)
{
    IActor::PreLoad(fbBuffer);

    ResourceManager& resMgr = (ResourceManager&)SystemLocator::GetInstance()->RequestSystem("ResourceManager");

    const FBFont* font = (FBFont*)fbBuffer;
    m_fontMesh = &resMgr.AcquireResource(font->FontResourceName()->c_str());
}

void FontActor::Load()
{
    IActor::Load();
}

void FontActor::UpdateState()
{
    if (m_fontMesh && m_fontMesh->GetResource<VectorFontMesh>()->IsLoaded())
    {
        m_actorState = IActor::ActorState_Loaded;
    }
    else if (m_fontMesh == nullptr)
    {
        m_actorState = IActor::ActorState_Loaded;
    }
}

void FontActor::Update(f32 dt)
{
    IActor::Update(dt);
}

void FontActor::Draw(RenderContext& context)
{
    IActor::Draw(context);

    m_fontMesh->GetResource<VectorFontMesh>()->DrawText("ABCD", XCVec3Unaligned(1.0f, 10.0f, 0.0f), context);
    //m_fontMesh->DrawText("0123456789", XCVec3Unaligned(1.0f, 0.0f, 0.0f), context);
}

void FontActor::Destroy()
{
    IActor::Destroy();
}
