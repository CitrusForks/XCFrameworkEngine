/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef __CUBEMAPPS_HLSL__
#define __CUBEMAPPS_HLSL__

#include "..\LightingShaders\DirectionalLight.hlsl"
#include "..\LightingShaders\PointLight.hlsl"
#include "..\LightingShaders\SpotLight.hlsl"

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

DepthStencilState LessEqualDSS
{
    DepthFunc = LESS_EQUAL;
};

float4 PSMain(VertexOut pin) : SV_TARGET
{
    return gCubeMap.Sample(samLinear, pin.PosW);
}

#endif