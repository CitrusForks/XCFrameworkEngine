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
}

void IActor::Load()
{
    m_invalidated = false;
    m_worldReady = true;
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

void IActor::Destroy()
{
    Invalidate();
    m_worldReady = false;
}

void IActor::Invalidate()
{
    m_invalidated = true;
}

