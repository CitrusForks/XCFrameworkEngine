/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef _LIGHTTEXTUREVS_H_
#define _LIGHTTEXTUREVS_H_

#include "..\LightingShaders\LightSource.hlsl"

struct PerObjectBuffer 
{
    float4x4    gWorld;
    float4x4    gWVP;
    float4x4    gWorldInvTranspose;
    float4x4    gTexTransform;
    Material    gMaterial;
};

cbuffer cbInstancedBuffer : register(b0)
{
    PerObjectBuffer gPerObject[100];
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
SamplerState    samLinear : register(s0);

struct VertexIn
{
    float3 PosL          : POSITION;
    float3 NormalL       : NORMAL;
    float2 Tex           : TEXCOORD;
    uint   InstanceIndex : SV_InstanceID;
};

struct VertexOut
{
    float4 PosH             : SV_POSITION;
    float3 PosW             : POSITION;
    float3 NormalW          : NORMAL;
    float2 Tex              : TEXCOORD;
};

VertexOut VSMain(VertexIn vin)
{
    VertexOut vout;

    // Transform to world space.
    vout.PosW = mul(float4(vin.PosL, 1.0), gPerObject[vin.InstanceIndex].gWorld).xyz;
    vout.PosH = mul(float4(vin.PosL, 1.0f), gPerObject[vin.InstanceIndex].gWVP);
    vout.NormalW = normalize(mul(float4(vin.NormalL, 0.0f), gPerObject[vin.InstanceIndex].gWorldInvTranspose)).xyz;
    vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gPerObject[vin.InstanceIndex].gTexTransform).xy;
    
    return vout;
}

#endif