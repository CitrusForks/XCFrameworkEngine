/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "SimpleActor.h"

SimpleActor::SimpleActor(void)
{
}

SimpleActor::~SimpleActor(void)
{
}

IActor::ActorReturnState SimpleActor::Init()
{
    return IActor::Init();
}

IActor::ActorReturnState SimpleActor::Update(f32 dt)
{
    return IActor::Update(dt);
}

bool SimpleActor::Draw(RenderContext& renderContext)
{
    return IActor::Draw(renderContext);
}

IActor::ActorReturnState SimpleActor::Destroy()
{
    return IActor::Destroy();
}