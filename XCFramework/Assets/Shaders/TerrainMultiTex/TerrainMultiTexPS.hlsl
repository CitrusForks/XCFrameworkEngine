/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef _TERRAINMULTITEXPS_H_
#define _TERRAINMULTITEXPS_H_

#include "..\LightingShaders\LightSource.hlsl"

cbuffer PerObjectBuffer : register(b0)
{
    float4x4    gWorld;
    float4x4    gWVP;
    float4x4    gWorldInvTranspose;
    float4x4    gTexTransform;
    Material    gMaterial;
};

#if defined(FORWARD_LIGHTING)
cbuffer cbLightsPerFrame : register(b1)
{
    LightSource      gLightSource[10];
    float4           gNoOfLights;
    float4           padding1;
    float4           padding2;
    float4           padding3;
};
#endif

Texture2D       gDiffuseMap : register(t0);    //Mapped with ShaderResource Variable
Texture2D       gDiffuseMap1 : register(t1);    //Mapped with ShaderResource Variable
Texture2D       gDiffuseMap2 : register(t2);
Texture2D       gBlendMap : register(t3);    //Mapped with ShaderResource Variable

SamplerState    samLinear : register( s0 );

struct VertexIn
{
    float3 PosL     : POSITION;
    float3 NormalL  : NORMAL;
    float2 Tex      : TEXCOORD;
};

struct VertexOut
{
    float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
    float3 NormalW  : NORMAL;
    float2 Tex      : TEXCOORD;
};

struct PixelOut
{
    float4 RTMain        : SV_Target0;
    float4 RTDiffuse     : SV_Target1;
    float4 RTPosition    : SV_Target2;
    float4 RTNormal      : SV_Target3;
};

PixelOut PSMain(VertexOut pin) : SV_Target
{
    //Interpolating normal can unmormalize it, so normalize it
    pin.NormalW = normalize(pin.NormalW);

    float4 texColor  = float4(1, 1, 1, 1);
    float4 texColor1 = float4(1, 1, 1, 1);
    float4 texColor2 = float4(1, 1, 1, 1);
    float4 texBlend  = float4(1, 1, 1, 1);
    
    texColor  = gDiffuseMap.Sample(samLinear, pin.Tex);
    texColor1 = gDiffuseMap1.Sample(samLinear, pin.Tex);
    texColor2 = gDiffuseMap2.Sample(samLinear, pin.Tex);
    
    texBlend = gBlendMap.Sample(samLinear, pin.Tex);
    
    float totalInverse = 1.0f / (texBlend.r + texBlend.g + texBlend.b);
    
    texColor *= texBlend.r * totalInverse;
    texColor1 *= texBlend.g * totalInverse;
    texColor2 *= texBlend.b * totalInverse;
    
    float4 finalColor = texColor + texColor1 + texColor2;

#if defined(FORWARD_LIGHTING)
    float4 lightImpact = float4(0, 0, 0, 0);

    for (unsigned int index = 0; index < gNoOfLights.x; ++index)
    {
        lightImpact += CalculateLightImpact(gLightSource[index], float4(pin.PosW, 0.0), float4(pin.NormalW, 0.0));
    }

    lightImpact = lightImpact / gNoOfLights.x;

    finalColor += lightImpact;

    //Add the ambient light
    finalColor += gMaterial.Ambient;

    finalColor.a = gMaterial.Diffuse.a * texColor.a;
#endif

    PixelOut outColors;
    outColors.RTMain     = finalColor;
    outColors.RTDiffuse  = finalColor;
    outColors.RTPosition = float4(pin.PosW, 0.0);
    outColors.RTNormal   = float4(pin.NormalW, 0.0);

    return outColors;
}

#endif