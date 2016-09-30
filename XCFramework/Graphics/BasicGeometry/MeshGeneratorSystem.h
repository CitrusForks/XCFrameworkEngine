/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/System/ISystem.h"

class MeshGeneratorSystem : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(MeshGeneratorSystem)

    MeshGeneratorSystem(void);
    virtual ~MeshGeneratorSystem(void);

    virtual void                    Init();
    virtual void                    Load();
    virtual void                    Destroy();
};