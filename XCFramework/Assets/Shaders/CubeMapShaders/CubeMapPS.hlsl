/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef _CUBEMAPPS_H_
#define _CUBEMAPPS_H_

cbuffer cbWVP : register(b0)
{
    float4x4 gWVP;
};

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
    float4 RTMain        : SV_Target0;
    float4 RTDiffuse     : SV_Target1;
    float4 RTPosition    : SV_Target2;
    float4 RTNormal      : SV_Target3;
};

TextureCube     gCubeMap;    //Mapped with ShaderResource Variable
SamplerState    samLinear : register(s0);

DepthStencilState LessEqualDSS
{
    DepthFunc = LESS_EQUAL;
};

PixelOut PSMain(VertexOut pin) : SV_TARGET
{
    float4 finalColor = gCubeMap.Sample(samLinear, pin.PosW);
    
    PixelOut outColors;
    outColors.RTMain     = finalColor;
    outColors.RTDiffuse  = finalColor;
    outColors.RTPosition = finalColor;
    outColors.RTNormal   = finalColor;

    return outColors;
}

#endif