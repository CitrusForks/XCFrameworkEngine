/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "IActor.h"
#include "Assets/Packages/PackageConsts.h"

IActor::IActor(void)
    : m_userFriendlyName("None")
    , m_actorReady(false)
    , m_invalidated(false)
    , m_actorState(ActorState_None)
{
}

void IActor::PreLoad(const void* fbBuffer)
{
    if (fbBuffer)
    {
        const FBIActor* iactorBuff = (FBIActor*)fbBuffer;

        m_currentPosition.SetValues(iactorBuff->Position()->x(), iactorBuff->Position()->y(), iactorBuff->Position()->z(), iactorBuff->Position()->w());
        m_initialRotation.SetValues(iactorBuff->Rotation()->x(), iactorBuff->Rotation()->y(), iactorBuff->Rotation()->z(), iactorBuff->Rotation()->w());
        m_initialScaling.SetValues(iactorBuff->Scaling()->x(), iactorBuff->Scaling()->y(), iactorBuff->Scaling()->z(), iactorBuff->Scaling()->w());

        m_material.Ambient = XCVec4Unaligned(iactorBuff->Material()->Ambient()->x(), iactorBuff->Material()->Ambient()->y(), iactorBuff->Material()->Ambient()->z(), iactorBuff->Material()->Ambient()->w());
        m_material.Diffuse = XCVec4Unaligned(iactorBuff->Material()->Diffuse()->x(), iactorBuff->Material()->Diffuse()->y(), iactorBuff->Material()->Diffuse()->z(), iactorBuff->Material()->Diffuse()->w());
        m_material.Specular = XCVec4Unaligned(iactorBuff->Material()->Specular()->x(), iactorBuff->Material()->Specular()->y(), iactorBuff->Material()->Specular()->z(), iactorBuff->Material()->Specular()->w());
    }

    m_actorState = ActorState_PreLoaded;
}

void IActor::Load()
{
    for (auto& actor : m_childNodes)
    {
        actor->Load();
    }
}

void IActor::UpdateState()
{
    if (!m_actorReady && m_actorState == ActorState_Loaded)
    {
        m_invalidated  = false;
        m_actorReady   = true;
        m_isRenderable = true;
    }
    else if (m_actorReady && m_actorState == ActorState_Unloaded)
    {
        Invalidate();
    }

    for (auto& actor : m_childNodes)
    {
        actor->UpdateState();
    }
}

IActor::~IActor(void)
{
    Logger("[IActor] Destructor called for %s", m_userFriendlyName.c_str());
}

void IActor::Update(f32 dt)
{
    for (auto& actor : m_childNodes)
    {
        actor->Update(dt);
    }
}

void IActor::Draw(RenderContext& renderContext)
{
    for (auto& actor : m_childNodes)
    {
        actor->Draw(renderContext);
    }
}

void IActor::Unload()
{
    for (auto& actor : m_childNodes)
    {
        actor->Unload();
    }

    m_actorState = ActorState_Unloaded;
}

void IActor::Destroy()
{
    for (auto& actor : m_childNodes)
    {
        actor->Destroy();
    }
}

void IActor::Invalidate()
{
    m_invalidated   = true;
    m_actorReady    = false;
    m_isRenderable  = false;

    for (auto& actor : m_childNodes)
    {
        actor->Invalidate();
    }
}