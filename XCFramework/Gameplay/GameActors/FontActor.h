/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "IActor.h"
#include "Engine/Graphics/XC_Mesh/XCMesh.h"
#include "Engine/Graphics/XC_Mesh/VectorFontMesh.h"

class FontActor : public IActor
{
public:
    DECLARE_OBJECT_CREATION(FontActor)

    FontActor();
    virtual ~FontActor(void);
    
    virtual void                        Init(int actorId);
    virtual void                        PreLoad(const void* fbBuffer);
    virtual void                        Load();
    virtual void                        Update(float dt);
    virtual void                        Draw(RenderContext& context);
    virtual void                        Destroy();
    
protected:
    VectorFontMesh*                     m_fontMesh;
};