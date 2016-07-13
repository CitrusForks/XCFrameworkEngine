/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "ICamera.h"

class GlobalCamera : public ICamera
{
public:

    void    SetViewMatrix(const XCMatrix4& view)       { m_viewMatrix = view; }
    void    SetProjectionMatrix(const XCMatrix4& proj) { m_projectionMatrix = proj; }

};