/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "..\LightingShaders\DirectionalLight.hlsl"
#include "..\LightingShaders\PointLight.hlsl"
#include "..\LightingShaders\SpotLight.hlsl"

cbuffer cbLightsPerFrame : register(b0)
{
    DirectionalLight gDirLight;
    PointLight       gPointLight;
    SpotLight        gSpotLight;
    float3           gEyePosW;
};

cbuffer PerObjectBuffer : register(b1)
{
    float4x4    gWorld;
    float4x4    gWVP;
    float4x4    gWorldInvTranspose;
    float4x4    gTexTransform;
    Material    gMaterial;
};

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


float4 PSMain(VertexOut pin) : SV_Target
{
    //Interpolating normal can unmormalize it, so normalize it
    pin.NormalW = normalize(pin.NormalW);

    float3 toEye = normalize(gEyePosW - pin.PosW);

    float4 texColor = float4(1, 1, 1, 1);
	float4 texColor1 = float4(1, 1, 1, 1);
	float4 texColor2 = float4(1, 1, 1, 1);
	float4 texBlend = float4(1, 1, 1, 1);

    texColor = gDiffuseMap.Sample(samLinear, pin.Tex);
	texColor1 = gDiffuseMap1.Sample(samLinear, pin.Tex);
	texColor2 = gDiffuseMap2.Sample(samLinear, pin.Tex);

	texBlend = gBlendMap.Sample(samLinear, pin.Tex);

	float totalInverse = 1.0f / (texBlend.r + texBlend.g + texBlend.b);

	texColor *= texBlend.r * totalInverse;
	texColor1 *= texBlend.g * totalInverse;
	texColor2 *= texBlend.b * totalInverse;

    float4 litColor = texColor + texColor1 + texColor2;

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

	/*if (pin.PosW.y > 0)
	{
		//litColor *= (ambient + diffuse) + specular;
		litColor = texColor * (ambient + diffuse) + specular;
		//Common to take alpha from diffuse material
		litColor.a = gMaterial.Diffuse.a * texColor.a;
	}
	else*/
	if (pin.PosW.y < 0)
	{
		litColor *= /*texColor1 * */(ambient + diffuse) + specular;
		//Common to take alpha from diffuse material
		litColor.a = gMaterial.Diffuse.a * texColor1.a;
	}
	else
	{
		litColor *= (ambient + diffuse) + specular;
		//Common to take alpha from diffuse material
		litColor.a = gMaterial.Diffuse.a * texColor.a;
	}

	return litColor;
}