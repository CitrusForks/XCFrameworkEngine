/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "AnimatedActor.h"


AnimatedActor::AnimatedActor(void)
{
}

AnimatedActor::~AnimatedActor(void)
{
}

IActor::IActor::ActorReturnState AnimatedActor::Init()
{
    return IActor::Init();
}

IActor::IActor::ActorReturnState AnimatedActor::LoadMetaData(const void* metaData)
{
    return IActor::LoadMetaData(metaData);
}

IActor::IActor::ActorReturnState AnimatedActor::Load()
{
    return IActor::Load();
}

IActor::IActor::ActorReturnState AnimatedActor::OnLoaded()
{
    return IActor::OnLoaded();
}

IActor::IActor::ActorReturnState AnimatedActor::Update(f32 dt)
{
    return IActor::Update(dt);
}

bool AnimatedActor::Draw(RenderContext& renderContext)
{
    return IActor::Draw(renderContext);
}

IActor::IActor::ActorReturnState AnimatedActor::Unload()
{
    return IActor::Unload();
}

IActor::IActor::ActorReturnState AnimatedActor::OnUnloaded()
{
    return IActor::OnUnloaded();
}

IActor::IActor::ActorReturnState AnimatedActor::Destroy()
{
    return IActor::Destroy();
}