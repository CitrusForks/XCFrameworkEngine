/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include <string.h>

//Data Schemas- which are auto generated
#include "Assets/Packages/Schema/BasicTypes_generated.h"
#include "Assets/Packages/Schema/ShaderTypes_generated.h"
#include "Assets/Packages/Schema/SceneNodeTypes_generated.h"
#include "Assets/Packages/Schema/Shaders_generated.h"
#include "Assets/Packages/Schema/Resources_generated.h"
#include "Assets/Packages/Schema/GameplayActors_generated.h"
#include "Assets/Packages/Schema/OpenWorldSceneGraph_generated.h"
#include "Assets/Packages/Schema/XCMeshJson_generated.h"

//Resource specifics
static const unsigned int NbOfTextureResources = 14;    //To add up SRV descriptors in SharedDescriptorHeap

#if defined(XC_ORBIS)

#include "Assets/Packages/PackageConstsOrbis.h"

#else
static const char* RESOURCE_SCHEMA_FOLDERPATH   = "Assets/Packages/Schema/";
static const char* RESOURCE_DATA_FOLDERPATH     = "Assets/Packages/Data/";

static const char* RESOURCE_SCHEMA_FILEPATH     = "Assets/Packages/Schema/Resources.schema";
static const char* RESOURCE_DATA_FILEPATH       = "Assets/Packages/Data/Resources.pak";

static const char* XCMESHJSON_SCHEMA_FILEPATH       = "Assets/Packages/Schema/XCMeshJson.schema";
static const char* TEST_XCMESHJSON_DATA_FILEPATH    = "Assets/Meshes/Soldiers/ArmyPilot/armypilot.xcmeshjson";

static const char* BASICTYPES_SCHEMA_FILEPATH = "Assets/Packages/Schema/BasicTypes.schema";
static const char* GAMEACTORS_SCHEMA_FILEPATH = "Assets/Packages/Schema/GameplayActors.schema";
static const char* SCENE_SCHEMA_FILEPATH      = "Assets/Packages/Schema/OpenWorldSceneGraph.schema";
static const char* SCENE_DATA_FILEPATH        = "Assets/Packages/Data/SceneGraph.pak";

static const char* SHADER_FOLDERPATH            = "Assets/Shaders/";
static const char* SHADER_SCHEMA_FILEPATH       = "Assets/Packages/Schema/Shaders.schema";
static const char* SHADER_DATA_FILEPATH         = "Assets/Packages/Data/Shaders.pak";

static std::string getPlatformPath(std::string inPath)
{
    //Not required on win32. Implemented on Orbis and durango
    return inPath;
}

#endif