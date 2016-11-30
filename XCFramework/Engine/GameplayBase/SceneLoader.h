/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/TaskManager/Task/AsyncTask.h"
#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"

class SceneGraph;

class SceneLoader : public AsyncTask
{
public:
    SceneLoader(SceneGraph& world, std::string packageData);
    ~SceneLoader();

    void                        Init() override;
    void                        Run()  override;

private:
    std::string                 m_packageName;
    SceneGraph&                 m_sceneRef;
    bool                        m_loadRequested;
};