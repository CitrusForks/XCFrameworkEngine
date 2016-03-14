/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "LiveDriveVRClient.h"

LiveDriveVRClient::LiveDriveVRClient()
{
    Logger("[Network] LiveDriveVRClient initialized");
    m_sendData = false;
}

LiveDriveVRClient::~LiveDriveVRClient()
{
}

void LiveDriveVRClient::init(const char *ip, int port)
{
    Client::init(ip, port);
    m_directInput = (DirectInput*) &SystemLocator::GetInstance()->RequestSystem("InputSystem");
    m_graphicsSystem = (XC_Graphics*) &SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
}

void LiveDriveVRClient::update()
{
    if (m_isConnectedToServer)
    {
        if (m_directInput->KeyDown(INPUT_RIGHT))
        {
            m_sendData = true;
        }
        sendTexture();
        receiveInputs();
    }
}

void LiveDriveVRClient::sendTexture()
{
    if (m_sendData)
    {
#if defined(XCGRAPHICS_DX11)
        RenderableTexture& texture = m_graphicsSystem->GetRenderTexture(RENDERTARGET_LIVEDRIVE);
        m_texInfo = texture.GetRenderToTexture();
        if (m_texInfo)
        {
            normalizeTexData();
            sendData("S", 1);
            sendData((char*)m_texInfo->m_texData, m_texInfo->m_texSize);
        }
#endif
    }
}

void LiveDriveVRClient::receiveInputs()
{
    char* data = receiveData();
    if (data[0] != '!')
    {
        //Logger("[Live Drive Received Inputs] %s ", data);
        processAccelerometerInputs(data);
    }
}

void LiveDriveVRClient::processAccelerometerInputs(const char* pData)
{
    unsigned int index = 0;
    
    while (pData[index] != '\0')
    {
        switch (pData[index++])
        {
            case ACCELEROMETER_UP :
                //m_gameSystem->GetInputSystem().->SetFakeKeyDown(DIK_W);
                m_directInput->SetFakeMouseInput(MOUSEINPUT_DY, 1);
                break;
            case ACCELEROMETER_DOWN:
                //m_gameSystem->GetInputSystem().->SetFakeKeyDown(DIK_S);
                m_directInput->SetFakeMouseInput(MOUSEINPUT_DY, -1);
                break;
            case ACCELEROMETER_LEFT:
                //m_gameSystem->GetInputSystem().->SetFakeKeyDown(DIK_A);
                m_directInput->SetFakeMouseInput(MOUSEINPUT_DX, 1);
                break;
            case ACCELEROMETER_RIGHT:
                //m_gameSystem->GetInputSystem().->SetFakeKeyDown(DIK_D);
                m_directInput->SetFakeMouseInput(MOUSEINPUT_DX, -1);
                break;
            case ACCELEROMETER_TOWARDS:
                //m_gameSystem->GetInputSystem().->SetFakeKeyDown(DIK_S);
                break;
            case ACCELEROMETER_AWAY:
                //m_gameSystem->GetInputSystem().->SetFakeKeyDown(DIK_W);
                break;
            default:
                break;
        }
    }
}

void LiveDriveVRClient::normalizeTexData()
{
#if defined(XCGRAPHICS_DX11)
    for (unsigned int index = 0; index < m_texInfo->m_texSize; index++)
    {
        if (m_texInfo->m_texData[index] == '\n' || m_texInfo->m_texData[index] == 0 || m_texInfo->m_texData[index] == '\r')
        {
            m_texInfo->m_texData[index] = 1;
        }
    }
#endif
}
