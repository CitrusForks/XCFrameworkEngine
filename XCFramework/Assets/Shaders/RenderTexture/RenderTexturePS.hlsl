/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#ifndef _RENDERTEXTUREPS_H_
#define _RENDERTEXTUREPS_H_

Texture2D gTexture : register(t0);
SamplerState samLinear : register(s0);

struct VSInput
{
    float3 Position : POSITION;
    float2 Tex      : TEXCOORD;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 Tex      : TEXCOORD;
};

struct PixelOut
{
    float4 RenderTarget0 : SV_Target0;
};

float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 finalColor = gTexture.Sample(samLinear, input.Tex);

    PixelOut outColors;
    outColors.RenderTarget0 = finalColor;

    return finalColor;
}

#endif