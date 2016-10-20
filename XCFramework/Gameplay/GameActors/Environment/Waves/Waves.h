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
 
    void                        PreLoad(const void* fbBuff) override;
    void                        Update(f32 dt) override;
    void                        Draw(RenderContext& context) override;
    void                        Destroy() override;
};