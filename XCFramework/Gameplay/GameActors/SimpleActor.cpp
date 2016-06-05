/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameActors/SimpleActor.h"

SimpleActor::SimpleActor(void)
{
}

SimpleActor::~SimpleActor(void)
{
}

void SimpleActor::Init(int actorId)
{
    IActor::Init(actorId);

    m_currentPosition = XMVectorZero();

    m_MTranslation = XMMatrixIdentity();
    m_MRotation = XMMatrixIdentity();
    m_MScaling = XMMatrixIdentity();
}

void SimpleActor::Update(float dt)
{
    IActor::Update(dt);
}

void SimpleActor::Draw(RenderContext& renderContext)
{
    IActor::Draw(renderContext);
}

void SimpleActor::Destroy()
{
    IActor::Destroy();
}