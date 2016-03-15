/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef __SPOTLIGHT__
#define __SPOTLIGHT__

#ifndef __MATERIAL__
#define __MATERIAL__
#include "..\Materials\BasicMaterial.hlsl"
#endif

struct SpotLight
{
    float4  Ambient;
    float4  Diffuse;
    float4  Specular;
    float3  Position;
    float   Range;
    float3  Direction;
    float   Spot;
    float3  Att;
    float   pad;
};

void ComputeSpotLight(Material mat, SpotLight L,
                      float3 pos, float3 normal, float3 toEye,
                      out float4 ambient, out float4 diffuse, out float4 spec)
{
    // Initialize outputs.
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // The vector from the surface to the light.
    float3 lightVec = L.Position - pos;
    
        // The distance from surface to light.
    float d = length(lightVec);
    
    // Range test.
    if( d > L.Range )
        return;
    
    // Normalize the light vector.
    lightVec /= d;
    
    // Ambient term.
    ambient = mat.Ambient * L.Ambient;
    
    // Add diffuse and specular term, provided the surface is in
    // the line of site of the light.
    float diffuseFactor = dot(lightVec, normal);
    
    // Flatten to avoid dynamic branching.
    [flatten]
    if(diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        spec = specFactor * mat.Specular * L.Specular;
    }
    // Scale by spotlight factor and attenuate.
    float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);

    // Scale by spotlight factor and attenuate.
    float att = spot / dot(L.Att, float3(1.0f, d, d*d));
    ambient *= spot;
    diffuse *= att;
    spec    *= att;
}

#endif