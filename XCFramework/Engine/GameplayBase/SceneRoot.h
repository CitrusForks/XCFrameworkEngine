/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "SceneNode.h"

#include "Engine/GameplayBase/Actors/IActor.h"

class SceneRoot : public IActor
{
public:
    DECLARE_OBJECT_CREATION(SceneRoot)

    SceneRoot()
    {
        //Keep it loaded for now.
        m_actorState = IActor::ActorState_PreLoaded;
    }

    ~SceneRoot() {}
};