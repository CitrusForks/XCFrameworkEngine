/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/System/ISystem.h"

#include "Gameplay/XCCamera/XC_CameraTypes.h"

#include "Graphics/XCGraphics.h"

class IActor;
class ICamera;

class XCCameraManager : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(XCCameraManager)

    XCCameraManager();
    ~XCCameraManager();

    void                   Init(XCGraphics& graphicsSystem, i32 clientWidth, i32 clientHeight);
    void                   Update(f32 dt);
    void                   Draw();
    void                   Destroy();

    void                   SetCameraType(ECameraType type) { m_currentCameraType = type; }
    ICamera*               GetCurrentCamera() { return m_Cameras[m_currentCameraType].get(); }

    void                   onResize(i32 clientWidth, i32 clientHeight);

    void                   AttachCameraToActor(IActor* actor);
    void                   AttachCameraToActor(IActor* actor, XCVec4& distanceFromTarget);

protected:
    XCMatrix4              GetViewMatrix() { return m_Cameras[m_currentCameraType]->GetViewMatrix(); }
    XCMatrix4              GetProjMatrix() { return m_Cameras[m_currentCameraType]->GetProjectionMatrix(); }
    XCVec4                 GetCameraPosition() { return m_Cameras[m_currentCameraType]->GetPosition(); }
    XCVec4                 GetCameraTarget() { return m_Cameras[m_currentCameraType]->GetTarget(); }

private:
    std::map<ECameraType, std::unique_ptr<ICamera>>     m_Cameras;
    ECameraType                                         m_currentCameraType;

    XCGraphics*                                        m_graphicsSystem;
};