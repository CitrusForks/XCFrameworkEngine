/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "GameActorTypes.h"

//A kind of precompiled header. Define all actors according to their sequence.
#include "Base/Serializer/IBase.h"

#include "Engine/GameplayBase/Actors/IActor.h"
#include "Engine/GameplayBase/Actors/SimpleActor.h"
#include "Engine/GameplayBase/Actors/SimpleMeshActor.h"
#include "Engine/GameplayBase/Actors/PhysicsActor.h"
#include "Engine/GameplayBase/Actors/FontActor.h"
#include "Engine/GameplayBase/Actors/AnimatedActor.h"
#include "Engine/GameplayBase/SceneRoot.h"

#include "Environment/Door/Door.h"
#include "Environment/Terrain/SimpleTerrain.h"
#include "Environment/Terrain/Terrain.h"
#include "Environment/Waves/Waves.h"
#include "Environment/SkyBox/SimpleSkyBox.h"

#include "Soldier/Soldier.h"
#include "Soldier/PCSoldier.h"
#include "Soldier/NPCSoldier.h"

#include "Weapons/Guns/Gun.h"
#include "Weapons/Bullets/Bullet.h"

#include "Vehicles/Cars/Car.h"
#include "Vehicles/Cars/PCCar.h"
#include "Vehicles/Cars/NPCCar.h"

#include "Graphics/BasicGeometry/TexturedPlane.h"
#include "Gameplay/LiveDriveInterface/LiveDriveTexturePlane.h"