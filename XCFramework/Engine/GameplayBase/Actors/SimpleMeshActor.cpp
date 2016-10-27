/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "SimpleMeshActor.h"


SimpleMeshActor::SimpleMeshActor(void)
{
}

SimpleMeshActor::~SimpleMeshActor(void)
{
}

void SimpleMeshActor::Init(i32 actorId)
{
    SimpleActor::Init(actorId);
}

void SimpleMeshActor::Update(f32 dt)
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