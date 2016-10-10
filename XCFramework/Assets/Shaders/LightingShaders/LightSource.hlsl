/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef _LIGHTSOURCE_H_
#define _LIGHTSOURCE_H_

#include "..\Materials\Material.hlsl"

//Constants
static const float gSoftenerA = 0.01;
static const float gSoftenerB = 0.02;
static const float gSoftenerC = 0.03;

struct LightSource
{
    float4 LightColor;  //Color of the light that emits
    float4 Direction;   //Direction the light is pointing at
    float4 Intensity;   //Intensity of light in all 3 axis
    float4 Position;    //Physical position of light
};

float4 CalculateLightImpact(LightSource lightSource, float4 vertexPos, float4 surfaceNormal)
{
    //The angle of incident on the surface is the intensity of light that hits the surface
    float4 incidentLight = normalize(lightSource.Position - vertexPos);

    //Find the distance between the light sourc and the vertex.
    float dist = distance(lightSource.Position, vertexPos);

    //Calculate the attenuation of light at this vertex based on the inver square distance from light source.
    // I = 1 / (a + bd + c * square(d))
    float intensityOfLight = 1 / (gSoftenerA + (gSoftenerB * dist) + (gSoftenerC * dist * dist));

    //Multiply the intensity of light 
    intensityOfLight = (float) intensityOfLight * lightSource.Intensity;

    //Dot can return negative value, which shouldn't be lit at all. So take the max.
    float angleOfIncidence = intensityOfLight * max(dot(surfaceNormal.xyz, incidentLight.xyz), 0.0);
    
    float4 surfaceColor = angleOfIncidence * lightSource.LightColor;

    return surfaceColor;
}

#endif