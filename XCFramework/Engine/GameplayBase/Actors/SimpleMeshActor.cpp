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

IActor::ActorReturnState SimpleMeshActor::Init()
{
    return SimpleActor::Init();
}

IActor::ActorReturnState SimpleMeshActor::Update(f32 dt)
{
    return SimpleActor::Update(dt);
}

bool SimpleMeshActor::Draw(RenderContext& renderContext)
{
    return SimpleActor::Draw(renderContext);
}

IActor::ActorReturnState SimpleMeshActor::Destroy()
{
    return SimpleActor::Destroy();
}