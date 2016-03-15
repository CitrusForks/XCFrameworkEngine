/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

struct VertexOut
{
    float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

float4 PSMain(VertexOut pin) : SV_Target
{
    return pin.Color;
}