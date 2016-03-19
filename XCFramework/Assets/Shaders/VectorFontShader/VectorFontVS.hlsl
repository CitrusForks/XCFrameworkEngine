/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

cbuffer cbPerObjectInstanced : register(b0)
{
    float4x4 gWVP[100];
};

struct VertexIn
{
    float4          PosL  : POSITION;
    float4          Color : COLOR;
    uint            InstanceIndex : SV_InstanceID;
};

struct VertexOut
{
    float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut VSMain(VertexIn vin)
{
    VertexOut vout = (VertexOut)0;

    vin.PosL.w = 1.0f;

    // Transform to homogeneous clip space.
    vout.PosH = mul(float4(vin.PosL), gWVP[vin.InstanceIndex]);

    // Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;

    return vout;
}