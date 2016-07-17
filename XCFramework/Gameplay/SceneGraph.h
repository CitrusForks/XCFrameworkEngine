/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/IActor.h"

class XC_Graphics;

class SceneGraph
{
public:
    SceneGraph();
    virtual ~SceneGraph();

    virtual void            Init() {}
    virtual void            Update(f32 dt) {}
    virtual void            Draw(XC_Graphics& graphics) {}
    virtual void            Destroy() {}

private:
};