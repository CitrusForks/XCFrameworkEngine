/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

cbuffer cbWVP : register(b0)
{
    float4x4 gWVP;
};


TextureCube     gCubeMap;    //Mapped with ShaderResource Variable
SamplerState    samLinear : register(s0);

struct VertexIn
{
    float3 PosL     : POSITION;
};

struct VertexOut
{
    float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
};

struct PixelOut
{
    float4 RenderTarget0 : SV_Target0;
    float4 RenderTarget1 : SV_Target1;
    float4 RenderTarget2 : SV_Target2;
};

DepthStencilState LessEqualDSS
{
    DepthFunc = LESS_EQUAL;
};

PixelOut PSMain(VertexOut pin) : SV_TARGET
{
    float4 finalColor = gCubeMap.Sample(samLinear, pin.PosW);
    
    PixelOut outColors;
    outColors.RenderTarget0 = finalColor;
    outColors.RenderTarget1 = finalColor;
    outColors.RenderTarget2 = finalColor;

    return outColors;
}