/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

//Basically, the way they are ordered also defines the rendering order of that worker.
enum RenderWorkerType
{
    WorkerType_SkyBox,

    WorkerType_Misc,
    WorkerType_XCMesh,
    WorkerType_Terrain,
    WorkerType_ResourceLoader,

    WorkerType_Max
};