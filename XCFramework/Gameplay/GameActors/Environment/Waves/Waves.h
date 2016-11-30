/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/Environment/Terrain/SimpleTerrain.h"

class Waves : public SimpleTerrain
{
public:
    DECLARE_OBJECT_CREATION(Waves)

    Waves(void);
    virtual ~Waves(void);
 
    IActor::ActorReturnState LoadMetaData(const void* metaData) override;
    IActor::ActorReturnState Update(f32 dt) override;
    bool                     Draw(RenderContext& renderContext) override;
    IActor::ActorReturnState Destroy() override;
};