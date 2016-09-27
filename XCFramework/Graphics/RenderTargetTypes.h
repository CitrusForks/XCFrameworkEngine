/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

enum RenderTargetsType
{
    RenderTargetType_Main_0,
    RenderTargetType_Main_1,

    //Deferred Lighting
    RenderTargetType_GBuffer_Diffuse,
    RenderTargetType_GBuffer_Position,
    RenderTargetType_GBuffer_Normal,

    //Debug RT
    RenderTargetType_Debug,

    //Live Drive RT
    RenderTargetType_LiveDrive,

    RenderTargetType_Max
};

static const std::vector<DXGI_FORMAT> gs_RenderTargetFormats = { 
   DXGI_FORMAT_R8G8B8A8_UNORM,      //RenderTargetType_Main_0
   DXGI_FORMAT_R8G8B8A8_UNORM,      //RenderTargetType_Main_1

   DXGI_FORMAT_R8G8B8A8_UNORM,      //RenderTargetType_GBuffer_Diffuse
   DXGI_FORMAT_R32G32B32A32_FLOAT,  //RenderTargetType_GBuffer_Position
   DXGI_FORMAT_R8G8B8A8_UNORM,      //RenderTargetType_GBuffer_Normal

   DXGI_FORMAT_R8G8B8A8_UNORM,      //RenderTargetType_Debug

   DXGI_FORMAT_R8G8B8A8_UNORM       //RenderTargetType_LiveDrive
};