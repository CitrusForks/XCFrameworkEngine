/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#ifndef _RENDERTEXTUREVS_H_
#define _RENDERTEXTUREVS_H_

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

VSOutput VSMain(VSInput input)
{
    VSOutput result;

    result.Position = float4(input.Position, 1.0);
    result.Tex = input.Tex;

    return result;
}

#endif