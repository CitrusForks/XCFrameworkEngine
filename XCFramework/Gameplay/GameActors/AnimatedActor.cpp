/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameActors/AnimatedActor.h"


AnimatedActor::AnimatedActor(void)
{
}

AnimatedActor::~AnimatedActor(void)
{
}

void AnimatedActor::Init(i32 actorId)
{
    IActor::Init(actorId);
}

void AnimatedActor::Update(f32 dt)
{
    IActor::Update(dt);
}

void AnimatedActor::Draw(RenderContext& context)
{
    IActor::Draw(context);
}

void AnimatedActor::Destroy()
{
    IActor::Destroy();
}