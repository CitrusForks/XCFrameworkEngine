/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

#include "IPhysicsFeature.h"

class StaticBody : public IPhysicsFeature
{
public:
    void                Init(const PhysicsDesc& desc) override;
    void                Update(f32 dtS) override;
    void                Draw(RenderContext& context) override;
private:

};