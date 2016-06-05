/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"
#include "SystemLocator.h"

SystemLocator* SystemLocator::ms_pSystemLocator = nullptr;

SystemLocator::SystemLocator(void)
{
    m_systemFactory = std::make_unique<SystemFactory>();
    m_systemFactory->InitFactory();
    
    //Create System Container
    m_systemContainer.Init(*m_systemFactory.get());
}

SystemLocator* SystemLocator::GetInstance()
{
    if (ms_pSystemLocator == nullptr)
    {
        ms_pSystemLocator = new SystemLocator;
    }
    return ms_pSystemLocator;
}

SystemLocator::~SystemLocator(void)
{
    delete(ms_pSystemLocator);
}

ISystem& SystemLocator::RequestSystem(std::string sysName)
{
    return m_systemContainer.GetSystem(sysName);
}

void SystemLocator::Destroy()
{
    m_systemContainer.Destroy();
}