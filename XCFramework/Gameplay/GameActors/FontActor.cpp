/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Gameplay/GameActors/FontActor.h"

FontActor::FontActor()
{
    m_actorType = GAMEACTOR_FONT;
    m_useRenderWorkerType = WorkerType_Misc;
    m_useRenderWorkerType = WorkerType_XCMesh;
}

FontActor::~FontActor(void)
{
}

void FontActor::Init(int actorId)
{
    IActor::Init(actorId);

    m_MTranslation = XMMatrixIdentity();
    m_MRotation = XMMatrixIdentity();
    m_MScaling = XMMatrixIdentity();
    m_transformedRotation = m_MRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;
}

void FontActor::PreLoad(const void* fbBuffer)
{
    ResourceManager& resMgr = (ResourceManager&)SystemLocator::GetInstance()->RequestSystem("ResourceManager");

    const FBFont* font = (FBFont*)fbBuffer;
    m_fontMesh = (VectorFontMesh*)resMgr.GetResource(font->FontResourceName()->c_str());
}

void FontActor::Load()
{
    IActor::Load();
}

void FontActor::Update(float dt)
{
    IActor::Update(dt);
}

void FontActor::Draw(RenderContext& context)
{
    IActor::Draw(context);



    m_fontMesh->DrawText("ABCDE AA BB CC DD CB DB EA", XCVec3Unaligned(0.0f, 10.0f, 0.0f), context, ShaderType_VectorFont);
    m_fontMesh->DrawText("AA BB CC DD CB DB EA AD", XCVec3Unaligned(10.0f, 0.0f, 0.0f), context, ShaderType_VectorFont);
}

void FontActor::Destroy()
{
    IActor::Destroy();
}
