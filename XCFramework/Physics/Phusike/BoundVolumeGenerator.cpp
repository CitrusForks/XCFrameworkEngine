/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#include "PhysicsPrecompiledHeader.h"

#include "BoundVolumeGenerator.h"
#include "HeightfieldBoundVolume.h"
#include "OBBBoundVolume.h"

IPhysicsBoundVolume* BoundVolumeGenerator::GenerateBoundVolume(const PhysicsDesc::BoundTypeDesc & desc)
{
    IPhysicsBoundVolume* outBound = nullptr;

    switch(desc.m_boundType)
    {
        case PhysicsBoundType_HeightField:
            {
                outBound = XCNEW(HeightfieldBoundVolume)(desc.m_boundDesc.m_heightFieldDesc);
                break;
            }

        case PhysicsBoundType_Box:
            {
                outBound = XCNEW(OBBBoundVolume)(desc.m_boundDesc.m_boundBoxDesc);
                break;
            }

        default:
            XCASSERTMSG(false, "Invalid bound volume type");
            break;
    }

    return outBound;
}
