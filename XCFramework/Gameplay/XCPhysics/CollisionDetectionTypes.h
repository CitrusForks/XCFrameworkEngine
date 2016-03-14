/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

enum ECollisionDetectionType
{
	COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX = 0x00000000,
    COLLISIONDETECTIONTYPE_BOUNDINGBOX         = 0x00000001,
    COLLISIONDETECTIONTYPE_TRIANGLE            = 0x00000002,
    COLLISIONDETECTIONTYPE_RAY                 = 0x00000003,

    COLLISIONDETECTIONTYPE_TERRAIN             = 0x00000004,
    COLLISIONDETECTIONTYPE_BULLET              = 0x00000005,

    COLLISIONDETECTIONTYPE_MAX                 = 0x7FFFFFFF,
};