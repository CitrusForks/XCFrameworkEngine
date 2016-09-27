/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "NetworkPrecompiledHeader.h"

#include "LiveDriveVRClient.h"

#if defined(XCGRAPHICS_DX11)
#include "Graphics/XC_Graphics.h"
#include "Graphics/XC_Textures/RenderableTexture.h"
#endif

#include "Engine/Input/Directinput.h"

LiveDriveVRClient::LiveDriveVRClient()
{
    Logger("[Network] LiveDriveVRClient initialized");
    m_sendData = false;
}

LiveDriveVRClient::~LiveDriveVRClient()
{
}

void LiveDriveVRClient::Init(const char *ip, i32 port)
{
    Client::Init(ip, port);
    m_directInput = (DirectInput*) &SystemLocator::GetInstance()->RequestSystem("InputSystem");

#if defined(XCGRAPHICS_DX11)
    m_graphicsSystem = (XC_Graphics*) &SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
#endif
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
        RenderableTexture& texture = m_graphicsSystem->GetRenderTexture(RenderTargetType_LiveDrive);
        m_texInfo = texture.GetRenderToTexture();
        if (m_texInfo)
        {
            normalizeTexData();
            SendData("S", 1);
            SendData((char*)m_texInfo->m_texData, m_texInfo->m_texSize);
        }
#endif
    }
}

void LiveDriveVRClient::receiveInputs()
{
    char* data = ReceiveData();
    if (data[0] != '!')
    {
        //Logger("[Live Drive Received Inputs] %s ", data);
        processAccelerometerInputs(data);
    }
}

void LiveDriveVRClient::processAccelerometerInputs(const char* pData)
{
    u32 index = 0;
    
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
    for (u32 index = 0; index < m_texInfo->m_texSize; index++)
    {
        if (m_texInfo->m_texData[index] == '\n' || m_texInfo->m_texData[index] == 0 || m_texInfo->m_texData[index] == '\r')
        {
            m_texInfo->m_texData[index] = 1;
        }
    }
#endif
}
