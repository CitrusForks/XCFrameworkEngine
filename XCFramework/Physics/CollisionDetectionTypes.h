/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

enum ECollisionDetectionType
{
	COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX = 1 << 0,
    COLLISIONDETECTIONTYPE_BOUNDINGBOX         = 1 << 1,
    COLLISIONDETECTIONTYPE_TRIANGLE            = 1 << 2,
    COLLISIONDETECTIONTYPE_RAY                 = 1 << 3,

    COLLISIONDETECTIONTYPE_TERRAIN             = 1 << 4,
    COLLISIONDETECTIONTYPE_BULLET              = 1 << 5,

    COLLISIONDETECTIONTYPE_MAX                 = 0x7FFFFFFF,
};