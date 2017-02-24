/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "DebugInterfaceFactory.h"

#include "IDebugFeature.h"
#include "DebugPhysics/DebugPhysicsOBB.h"

#include "Base/Serializer/BaseIDGenerator.h"

DebugInterfaceFactory::DebugInterfaceFactory()
{
}

DebugInterfaceFactory::~DebugInterfaceFactory()
{
}

void DebugInterfaceFactory::InitFactory()
{
    RegisterDebugFeatures();
}

void DebugInterfaceFactory::RegisterDebugFeatures()
{
#if defined(DEBUG_PHYSICS_OBB)
    RegisterObject<DebugPhysicsOBB>("DebugPhysicsOBB");
#endif
}

IDebugFeature* DebugInterfaceFactory::CreateDebugFeature(std::string featureName)
{
    BaseIDGenerator& baseIdGen = SystemLocator::GetInstance()->RequestSystem<BaseIDGenerator>("BaseIDGenerator");
    u32 baseId = baseIdGen.GetNextBaseObjectId();

    IDebugFeature* feature = (IDebugFeature*) CreateObject(featureName);
    feature->SetBaseObjectId(baseId);

    Logger("[DEBUG FEATURE FACTORY] Feature : %s  ID : %d", featureName.c_str(), baseId);

    return feature;
}

void DebugInterfaceFactory::DestroyFactory()
{
}

#if defined(EDITOR)

extern "C" __declspec(dllexport) void GetAllActorTypes(GameActorsInfo* info)
{
    GAMEACTORS_FACTORY->GetAllActorTypes(info);
}

extern "C" __declspec(dllexport) i32 GetNoOfActorTypes()
{
    return GAMEACTORS_FACTORY->GetNoOfActorTypes();
}

#endif