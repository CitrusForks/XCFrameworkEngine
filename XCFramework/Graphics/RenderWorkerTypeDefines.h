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

    //For multiple passes
    WorkerType_PosDiffuseTex,
    WorkerType_Lighting,

    //For resource loading from the main graphics command list
    WorkerType_ResourceLoader,

    WorkerType_Max
};

enum RenderWorkerMask
{
    //For multiple passes
    WorkerMask_None                = 1,
    WorkerMask_PosDiffuseTex_Pass1 = 1 << 1,
    WorkerMask_Lighting_Pass2      = 1 << 2,
    WorkerMask_InstancedDraw       = 1 << 3,

    WorkerMask_Max                 = WorkerMask_InstancedDraw << 1
};

#if defined(UNUSED)
class RenderWorkerType
{
public:
    enum WorkerType
    {
        //For multiple passes
        WorkerType_PosDiffuseTex  = 1 << 0,
        WorkerType_Lighting       = 1 << 1,

        //For resource loading from the main graphics command list
        WorkerType_ResourceLoader = 1 << 2,

        WorkerType_SkyBox         = 1 << 3,
        WorkerType_Misc           = 1 << 4,

        WorkerType_Max            = 5
    };

    RenderWorkerType(WorkerType type)
        : m_workerType(type)
    {}

    u32 GetPosition()
    {
        u32 pos = 1;
        for (u32 mask = 1; mask <= WorkerType_Max; mask <<= 1)
        {
            if (mask & m_workerType)
            {
                break;
            }
            pos++;
        }
        return pos;
    }

    operator RenderWorkerType()
    {
        return GetPosition();
    }

private:
    WorkerType                  m_workerType;
};

#endif