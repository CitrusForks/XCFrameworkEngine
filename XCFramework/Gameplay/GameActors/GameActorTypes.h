/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

enum EGameActorType
{
    //Base types
    GAMEACTOR_IACTOR,

    GAMEACTOR_SIMPLE,
    GAMEACTOR_SIMPLEMESH,
    GAMEACTOR_PHYSICS,
    GAMEACTOR_ANIMATED,
    GAMEACTOR_FONT,

    //Basic Geometry
    GAMEACTOR_BOXGEOMETRY,

    //Game specific actors
    //Environments
    GAMEACTOR_DOOR,
    GAMEACTOR_WAVES,

    //Terrains
    GAMEACTOR_SIMPLETERRAIN,
    GAMEACTOR_TERRAIN,

    //Characters
    GAMEACTOR_SOLDIER,
    GAMEACTOR_PCSOLDIER,
    GAMEACTOR_NPCSOLDIER,

    //Weapons
    GAMEACTOR_GUN,

    //VEHICLES
    GAMEACTOR_CAR,
    GAMEACTOR_PCCAR,
    GAMEACTOR_NPCCAR,
    GAMEACTOR_BULLET,

    GAMEACTOR_MAX
};