/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

#include "PhysicsBoundVolumeGenerator.h"

//This Generator takes in BoundDesc from PhysicsDesc and create a bound covering the object.
class BoundVolumeGenerator : public PhysicsBoundVolumeGenerator
{
public:
    IPhysicsBoundVolume*    GenerateBoundVolume(const PhysicsDesc::BoundTypeDesc& desc);

private:

};