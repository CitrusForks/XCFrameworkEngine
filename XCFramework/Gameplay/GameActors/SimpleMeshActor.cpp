/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameActors/SimpleMeshActor.h"


SimpleMeshActor::SimpleMeshActor(void)
{
}

SimpleMeshActor::~SimpleMeshActor(void)
{
}

void SimpleMeshActor::Init(int actorId)
{
    SimpleActor::Init(actorId);
}

void SimpleMeshActor::Update(float dt)
{
    SimpleActor::Update(dt);
}

void SimpleMeshActor::Draw(RenderContext& renderContext)
{
    SimpleActor::Draw(renderContext);
}

void SimpleMeshActor::Destroy()
{
    SimpleActor::Destroy();
}