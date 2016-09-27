/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef _DEFERREDVS_H_
#define _DEFERREDVS_H_

#include "..\LightingShaders\LightSource.hlsl"

cbuffer cbLightsPerFrame : register(b0)
{
    LightSource      gLightSource[10];
    float4           gNoOfLights;
    float4           padding1;
    float4           padding2;
    float4           padding3;
};

struct VSInput
{
    float3 Position : POSITION;
    float2 Tex      : TEXCOORD;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 Tex      : TEXCOORD;
};

Texture2D       gGBufferDiffuse   : register(t0);
Texture2D       gGBufferPosition  : register(t1);
Texture2D       gGBufferNormal    : register(t2);

SamplerState    samLinear : register(s0);

VSOutput VSMain(VSInput input)
{
    VSOutput result;

    result.Position = float4(input.Position, 1.0);
    result.Tex = input.Tex;

    return result;
}

#endif