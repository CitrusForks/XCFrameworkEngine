/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "..\LightingShaders\DirectionalLight.hlsl"
#include "..\LightingShaders\PointLight.hlsl"
#include "..\LightingShaders\SpotLight.hlsl"

cbuffer cbSkinnedCharacterBuffer : register(b0)
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

cbuffer cbBoneBuffer : register(b2)
{
    float4x4    gBoneMatrix[60];
};


Texture2D       gDiffuseMap : register(t0);    //Mapped with ShaderResource Variable
SamplerState	samLinear : register(s0);

struct VertexIn
{
    float3 PosL         : POSITION;
    float3 NormalL      : NORMAL;
    float2 Tex          : TEXCOORD;
    float4 BlendIndices : BLENDINDICES;
    float4 BlendWeights : BLENDWEIGHT;
};

struct VertexOut
{
	float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
    float3 NormalW  : NORMAL;
    float2 Tex      : TEXCOORD;
};


float4 PSMain(VertexOut pin) : SV_Target
{
    //Interpolating normal can unmormalize it, so normalize it
    pin.NormalW = normalize(pin.NormalW);

    float3 toEye = normalize(gEyePosW - pin.PosW);

    float4 texColor = float4(1, 1, 1, 1);

    texColor = gDiffuseMap.Sample(samLinear, pin.Tex);
    
    float4 litColor = texColor;

    //Start with a sum of zero
    float4 ambient  = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse  = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    //Sum the light contribution from each light source.
    float4 A, D, S;

    ComputeDirectionalLight(gMaterial, gDirLight, pin.NormalW, toEye, A, D, S);
    ambient += A;
    diffuse += D;
    specular+= S;

    ComputePointLight(gMaterial, gPointLight, pin.PosW, pin.NormalW, toEye, A, D, S);
    ambient += A;
    diffuse += D;
    specular+= S;

    ComputeSpotLight(gMaterial, gSpotLight, pin.PosW, pin.NormalW, toEye, A, D, S);
    ambient += A;
    diffuse += D;
    specular+= S;

    litColor = texColor * (ambient + diffuse) + specular;

    //Common to take alpha from diffuse material
    litColor.a = gMaterial.Diffuse.a * texColor.a;
    
    return litColor;
}