/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/System/ISystem.h"

class IDebugFeature;

class DebugInterface : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(DebugInterface);

    DebugInterface();
    ~DebugInterface();

    void                          Init();
    void                          Update(float dt);
    void                          Destroy();

    void                          AddDebugFeature(std::string featureName);
    void                          RemoveDebugFeature(std::string featureName);

private:
    std::map<std::string, IDebugFeature*>   m_debugFeatures;
};