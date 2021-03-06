/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef _TERRAINMULTITEXVS_H_
#define _TERRAINMULTITEXVS_H_

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

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;

    AddressU = WRAP;
    AddressV = WRAP;
};

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

VertexOut VSMain(VertexIn vin)
{
    VertexOut vout;

    vout.PosW = mul(float4(vin.PosL, 1.0), gWorld).xyz;

    // Transform to homogeneous clip space.
    vout.PosH = mul(float4(vin.PosL, 1.0), gWVP);
    vout.NormalW = normalize(mul(float4(vin.NormalL, 0.0f), gWorldInvTranspose)).xyz;
    vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
    
    return vout;
}

#endif