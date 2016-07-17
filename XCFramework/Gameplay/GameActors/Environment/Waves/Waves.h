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
 
    virtual void                        Preload(const void* fbBuff);
    virtual void                        PreLoad(XCVec3& _initialPosition, i32 _rows, i32 _column, f32 _rowSpacing, f32 _colSpacing);
    virtual void                        Update(f32 dt);
    virtual void                        Draw(RenderContext& context);
    virtual void                        Destroy();
};