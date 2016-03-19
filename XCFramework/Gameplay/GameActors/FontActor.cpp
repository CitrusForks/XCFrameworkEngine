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
    m_fontMesh->CreateBuffers(VertexFormat_PositionColorInstanceIndex);
}

void FontActor::Update(float dt)
{
    IActor::Update(dt);
}

void FontActor::Draw(RenderContext& context)
{
    IActor::Draw(context);

    context.SetRasterizerState(RASTERIZERTYPE_FILL_SOLID);
    context.ApplyShader(SHADERTYPE_VECTORFONT);

    m_fontMesh->DrawText("EEEE AAAA BCA", XCVec3Unaligned(0.0f, 0.0f, 0.0f), context, SHADERTYPE_VECTORFONT);
}

void FontActor::Destroy()
{
    IActor::Destroy();
}
