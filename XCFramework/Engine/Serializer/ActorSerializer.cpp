/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "ActorSerializer.h"
#include "Engine/GameplayBase/Actors/GameActorsFactory.h"

IActor* ActorSerializer::ActorSpawner(std::string actorName, const void* nodeInstance)
{
    GameActorsFactory& actorFactory = SystemLocator::GetInstance()->RequestSystem<GameActorsFactory>("GameActorsFactory");

    IActor* actor = actorFactory.CreateActor(actorName);
    if (actor)
    {
        actor->SetMetaData(nodeInstance);
    }

    return actor;
}

std::string ActorSerializer::GetActorName(const FBSceneNode* node)
{
    //We remove the prefix "FB" from the enum names.
    std::string nodeName = EnumNameFBGenericSceneNode(node->NodeInstance_type());
    XCASSERT(strcmp(nodeName.substr(0, 2).c_str(), "FB") == 0);

    nodeName = nodeName.substr(2, nodeName.length() - 2);
    Logger("[SceneLoader]ActorName %s", nodeName.c_str());

    return nodeName;
}