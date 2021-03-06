/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Network/Client.h"

#if defined(XCGRAPHICS_DX11)
#include "Graphics/XCTextures/RenderableTexture.h"
#endif

class XCGraphics;
class XCInput;

static const char* IP_ADDRESS = "192.168.1.101";
static const i32   DEFAULT_PORT = 38300;

enum EAccelerometerInputType
{
    ACCELEROMETER_NONE      =  0 ,
    ACCELEROMETER_UP        = 'U',
    ACCELEROMETER_DOWN      = 'D',
    ACCELEROMETER_LEFT      = 'L',
    ACCELEROMETER_RIGHT     = 'R',
    ACCELEROMETER_TOWARDS   = 'T',
    ACCELEROMETER_AWAY      = 'W'
};

class LiveDriveVRClient : public Client
{
public:

    LiveDriveVRClient();
    virtual ~LiveDriveVRClient();

    virtual void     Init(const char *ip, i32 port);
    virtual void     update();

    void             sendTexture();
    void             receiveInputs();

    void             processAccelerometerInputs(const char* pData);

private:
    void             normalizeTexData();

#if defined(XCGRAPHICS_DX11)
    RenderableTexture::RenderedTextureInfo* m_texInfo;
    XCGraphics*                            m_graphicsSystem;
#endif

    bool                                    m_sendData;
    XCInput*                            m_directInput;
};