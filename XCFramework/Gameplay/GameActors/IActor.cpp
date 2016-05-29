/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Gameplay/GameActors/IActor.h"

IActor::IActor(void)
{
    m_look = XMVectorZero();
    m_right = XMVectorZero();
    m_up = XMVectorZero();

    m_worldReady = false;
    m_invalidated = false;
    m_isRenderable = false;

    m_actorState = ActorState_None;
}

void IActor::PreLoad(const void* fbBuffer)
{
    m_actorState = ActorState_Loading;
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

void IActor::Update(float dt)
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