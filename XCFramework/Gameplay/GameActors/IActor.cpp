/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameActors/IActor.h"
#include "Assets/Packages/PackageConsts.h"

IActor::IActor(void)
{
    m_worldReady = false;
    m_invalidated = false;
    m_isRenderable = false;

    m_actorState = ActorState_None;
}

void IActor::PreLoad(const void* fbBuffer)
{
    m_actorState = ActorState_Loading;

    const FBIActor* iactorBuff = (FBIActor*)fbBuffer;

    m_currentPosition.SetValues(iactorBuff->Position()->x(), iactorBuff->Position()->y(), iactorBuff->Position()->z(), iactorBuff->Position()->w());
    m_initialRotation.SetValues(iactorBuff->Rotation()->x(), iactorBuff->Rotation()->y(), iactorBuff->Rotation()->z(), iactorBuff->Rotation()->w());
    m_initialScaling.SetValues(iactorBuff->Scaling()->x(), iactorBuff->Scaling()->y(), iactorBuff->Scaling()->z(), iactorBuff->Scaling()->w());

    m_material.Ambient  = XCVec4Unaligned(iactorBuff->Material()->Ambient()->x(), iactorBuff->Material()->Ambient()->y(), iactorBuff->Material()->Ambient()->z(), iactorBuff->Material()->Ambient()->w());
    m_material.Diffuse  = XCVec4Unaligned(iactorBuff->Material()->Diffuse()->x(), iactorBuff->Material()->Diffuse()->y(), iactorBuff->Material()->Diffuse()->z(), iactorBuff->Material()->Diffuse()->w());
    m_material.Specular = XCVec4Unaligned(iactorBuff->Material()->Specular()->x(), iactorBuff->Material()->Specular()->y(), iactorBuff->Material()->Specular()->z(), iactorBuff->Material()->Specular()->w());
}

void IActor::Load()
{
}

void IActor::UpdateState()
{
    if (!m_worldReady && m_actorState == ActorState_Loaded)
    {
        m_invalidated = false;
        m_worldReady = true;
        m_isRenderable = true;
    }
    else if (m_worldReady && m_actorState == ActorState_Unloaded)
    {
        Invalidate();
    }
}

IActor::~IActor(void)
{
    Logger("[IActor] Destructor called for %s", m_userFriendlyName.c_str());
}

void IActor::Update(f32 dt)
{
}

void IActor::Draw(RenderContext& renderContext)
{
}

void IActor::Unload()
{
    m_actorState = ActorState_Unloaded;
}

void IActor::Destroy()
{
}

void IActor::Invalidate()
{
    m_invalidated   = true;
    m_worldReady    = false;
    m_isRenderable  = false;
}