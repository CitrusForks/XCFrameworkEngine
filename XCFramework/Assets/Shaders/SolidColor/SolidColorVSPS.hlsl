/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

cbuffer cbWorld : register(b0)
{
    float4x4 gWorld; 
};

cbuffer cbView : register(b1)
{
    float4x4 gView; 
};

cbuffer cbProjection : register(b2)
{
    float4x4 gProj; 
};

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut SolidColor_VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0;

	// Transform to homogeneous clip space.
    vout.PosH = mul(float4(vin.PosL, 1.0), gWorld);
    vout.PosH = mul(vout.PosH, gView);
	vout.PosH = mul(vout.PosH , gProj);

	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;

    return vout;
}

float4 SolidColor_PS(VertexOut pin) : SV_Target
{
    return pin.Color;
}