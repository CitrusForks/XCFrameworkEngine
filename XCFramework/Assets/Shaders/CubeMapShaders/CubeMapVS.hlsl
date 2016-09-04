/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

cbuffer cbWVP : register(b0)
{
    float4x4 gWVP; 
};

TextureCube     gCubeMap;    //Mapped with ShaderResource Variable
SamplerState    samLinear : register(s0);

struct VertexIn
{
    float3 PosL     : POSITION;
};

struct VertexOut
{
    float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
};

VertexOut VSMain(VertexIn vin)
{
    VertexOut vout;

    // Transform to homogeneous clip space.
    //set z/w = 1, because far plane
    vout.PosH = mul(float4(vin.PosL, 1.0), gWVP).xyww;
    vout.PosW = vin.PosL;

    return vout;
}
