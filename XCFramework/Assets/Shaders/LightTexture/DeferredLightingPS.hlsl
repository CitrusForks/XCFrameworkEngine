/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef _DEFERREDLIGHTINGPS_H_
#define _DEFERREDLIGHTINGPS_H_

#include "..\LightingShaders\LightSource.hlsl"

cbuffer cbLightsPerFrame : register(b0)
{
    LightSource      gLightSource[10];
    float4           gNoOfLights;
    float4           padding1;
    float4           padding2;
    float4           padding3;
};

Texture2D       gGBufferDiffuse   : register(t0);
Texture2D       gGBufferPosition  : register(t1);
Texture2D       gGBufferNormal    : register(t2);

SamplerState    samLinear : register(s0);

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

struct PixelOut
{
    float4 RTMain  : SV_Target0;
};

PixelOut PSMain(VSOutput pin) : SV_Target
{
    float4 diffuseColor   = gGBufferDiffuse.Sample(samLinear, pin.Tex);
    float4 vertexPosition = float4(gGBufferPosition.Sample(samLinear, pin.Tex).xyz, 0);
    float4 surfaceNormal  = gGBufferNormal.Sample(samLinear, pin.Tex);

    vertexPosition.w = 0.0;
    surfaceNormal.w  = 0.0f;

    float4 finalColor = diffuseColor;

    float4 lightImpact = float4(0, 0, 0, 0);
    
    for (unsigned int index = 0; index < gNoOfLights.x; ++index)
    {
        lightImpact += CalculateLightImpact(gLightSource[index], vertexPosition, surfaceNormal);
    }

    lightImpact = lightImpact / gNoOfLights.x;

    finalColor += lightImpact;

    //Take the diffuse alpha
    finalColor.a = diffuseColor.a;

    PixelOut outColors;
    outColors.RTMain  = finalColor;

    return outColors;
}

#endif