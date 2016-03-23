/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef __DIRECTIONALLIGHT__
#define __DIRECTIONALLIGHT__

#ifndef __MATERIAL__
#define __MATERIAL__
#include "..\Materials\BasicMaterial.hlsl"
#endif

struct DirectionalLight
{
    float4  Ambient;
    float4  Diffuse;
    float4  Specular;
    float3  Direction;
};


void ComputeDirectionalLight(Material mat, DirectionalLight L, float3 normal, float3 toEye,
                             out float4 ambient,
                             out float4 diffuse,
                             out float4 specular)
{
    //Init o/p
    ambient  = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse  = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    //The light vector aims opposite the direction the light rays travel.
    float3 lightVec = -L.Direction;

    //Add ambient term
    ambient = mat.Ambient * L.Ambient;

    //Add diffuse and specular term, provided the surface is in the line of sight
    //float lightIntensity = max(dot(lightVec, normal), 0.0f);
    float lightIntensity = dot(lightVec, normal);

    if(lightIntensity > 0.0f)
    {
        diffuse = lightIntensity * (mat.Diffuse * L.Diffuse);
    }
    //float3 v = reflect(-lightVec, normal);
    //float specFactor = pow( max( dot(v, toEye), 0.0f), mat.Specular.w);
    /*
    //Flatten to avoid dynamic branching
    [flatten]
    if(diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow( max( dot(v, toEye), 0.0f), mat.Specular.w);

        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        specular = specFactor * mat.Specular * L.Specular; 
    }*/
}

#endif