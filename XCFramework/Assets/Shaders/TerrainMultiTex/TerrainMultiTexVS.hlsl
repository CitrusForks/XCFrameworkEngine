/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "..\LightingShaders\DirectionalLight.hlsl"
#include "..\LightingShaders\PointLight.hlsl"
#include "..\LightingShaders\SpotLight.hlsl"

cbuffer cbPerObjectBuffer : register(b0)
{
    float4x4    gWorld;
    float4x4    gWVP;
    float4x4    gWorldInvTranspose;
    float4x4    gTexTransform;
    Material    gMaterial;
};

cbuffer cbLightsPerFrame : register(b1)
{
    DirectionalLight gDirLight;
    PointLight       gPointLight;
    SpotLight        gSpotLight;
    float3           gEyePosW;
};

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
