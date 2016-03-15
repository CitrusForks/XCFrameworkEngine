/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

cbuffer cbWVP : register(b0)
{
    float4x4 gWVP;
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

VertexOut VSMain(VertexIn vin)
{
    VertexOut vout = (VertexOut)0;

    // Transform to homogeneous clip space.
    vout.PosH = mul(float4(vin.PosL, 1.0), gWVP);

    // Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;

    return vout;
}