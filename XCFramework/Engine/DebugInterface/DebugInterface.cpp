/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "DebugInterface.h"
#include "DebugInterfaceFactory.h"

#include "IDebugFeature.h"
#include "DebugPhysics/DebugPhysicsOBB.h"

DebugInterface::DebugInterface()
{
}

DebugInterface::~DebugInterface()
{
}

void DebugInterface::Init()
{
    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RegisterSystem<DebugInterfaceFactory>("DebugInterfaceFactory");
    
    DebugInterfaceFactory& factory = (DebugInterfaceFactory&) container.CreateNewSystem("DebugInterfaceFactory");
    factory.InitFactory();

    for(auto& debugFeature : m_debugFeatures)
    {
        debugFeature.second->Init();
    }
}

void DebugInterface::Update(float dt)
{
    for(auto& debugFeature : m_debugFeatures)
    {
        debugFeature.second->Update(dt);
    }
}

void DebugInterface::Destroy()
{
    for(auto& debugFeature : m_debugFeatures)
    {
        debugFeature.second->Destroy();
        delete debugFeature.second;
    }

    m_debugFeatures.clear();

    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RemoveSystem("DebugInterfaceFactory");
}

void DebugInterface::AddDebugFeature(std::string featureName)
{
    auto& feature = std::find_if(m_debugFeatures.begin(), m_debugFeatures.end(), [featureName](const std::pair<std::string, IDebugFeature*> obj) -> bool
    {
        return featureName == obj.first;
    });

    if(feature == m_debugFeatures.end())
    {
        DebugInterfaceFactory& factory = SystemLocator::GetInstance()->RequestSystem<DebugInterfaceFactory>("DebugInterfaceFactory");
        m_debugFeatures[featureName] = factory.CreateDebugFeature(featureName);
        m_debugFeatures[featureName]->Init();
    }
    else
    {
        XCASSERTMSG("Trying to add a feature which is already added", false);
    }
}

void DebugInterface::RemoveDebugFeature(std::string featureName)
{
    auto& feature = std::find_if(m_debugFeatures.begin(), m_debugFeatures.end(), [featureName](const std::pair<std::string, IDebugFeature*> obj) -> bool
    {
        return featureName == obj.first;
    });

    if(feature != m_debugFeatures.end())
    {
        m_debugFeatures[featureName]->Destroy();
        delete m_debugFeatures[featureName];
        m_debugFeatures.erase(feature);
    }
    else
    {
        XCASSERTMSG("Trying to remove a feature which does not exist", false);
    }

}