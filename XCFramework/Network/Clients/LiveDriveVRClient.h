/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Network/Client.h"

class DirectInput;
class XC_Graphics;

static const char* IP_ADDRESS = "192.168.1.101";
static const int   DEFAULT_PORT = 38300;

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

    virtual void     Init(const char *ip, int port);
    virtual void     update();

    void             sendTexture();
    void             receiveInputs();

    void             processAccelerometerInputs(const char* pData);

private:
    void             normalizeTexData();

#if defined(XCGRAPHICS_DX11)
    RenderableTexture::RenderedTextureInfo* m_texInfo;
    XC_Graphics*                            m_graphicsSystem;
#endif

    bool                                    m_sendData;
    DirectInput*                            m_directInput;
};