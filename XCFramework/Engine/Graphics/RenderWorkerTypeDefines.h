/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

enum RenderWorkerType
{
    WorkerType_SkyBox,
    WorkerType_Terrain,
    WorkerType_Misc,
    WorkerType_ResourceLoader,

    WorkerType_Max
};