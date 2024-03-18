#version 460

#extension GL_ARB_shading_language_include : enable

//////////// ShadingRateSample ////////////
//
// The extension needs to get enabled in the Fragment Shader
//
#extension GL_NV_shading_rate_image : enable


#include "common.h"
#include "noise.glsl"

// inputs in view space
in Interpolants {
  centroid vec3 model_pos;
  centroid vec3 normal;
  centroid vec3 eyeDir;
  centroid vec3 lightDir;
} IN;

layout(location=0, index=0) out vec4 out_Color;


float calcNoise(vec3 modelPos, int iterations)
{  
  float val = 0;
  for ( int i = 0; i < iterations; ++i )
  {
    val += SimplexPerlin3D(modelPos*20) / iterations;
  }
  val = smoothstep(-0.1, 0.1, val);
  return val;
}

vec4 calculateLight(vec3 normal, vec3 eyeDir, vec3 lightDir, vec3 objColor) 
{
  // ambient term
  vec4 ambient_color = vec4( objColor * 0.25, 1.0 );
  
  // diffuse term
  float diffuse_intensity = max(dot(normal, lightDir), 0.0)/1.5;
  vec4  diffuse_color = diffuse_intensity * vec4(objColor, 1.0);
  
  // specular term
  vec3  R = reflect( -lightDir, normal );
  float specular_intensity = max( dot( eyeDir, R ), 0.0 );
  vec4  specular_color = pow(specular_intensity, 10) * vec4(0.8,0.8,0.8,1);
  
  return ambient_color + diffuse_color + specular_color;
}

void main()
{
  // interpolated inputs in view space
  vec3 normal   = normalize(IN.normal);
  vec3 eyeDir   = normalize(IN.eyeDir);
  vec3 lightDir = normalize(IN.lightDir);

  float noiseVal = calcNoise(IN.model_pos/2, scene.fragmentLoadFactor * 100);
//  vec3 objColor = object.color * (1 - noiseVal * 0.9f);
  vec3 objColor = object.color + vec3(noiseVal);

  out_Color = calculateLight(normal, eyeDir, lightDir, objColor);
    
  //////////// ShadingRateSample ////////////
  // 
  // The Fragemnt Shader exposes new build-in types
  // to query the shading rate for the current fragment.
  // Here we use them to visualize the shading rate.
  //
  if (scene.visualizeShadingRate == 1)
  {
    out_Color = vec4(1,0,0,1);
    int maxCoarse = max( gl_FragmentSizeNV.x, gl_FragmentSizeNV.y );

    if (maxCoarse == 1)
    {
      out_Color = vec4(1,0,0,1);
    } 
    else if (maxCoarse == 2)
    {
      out_Color = vec4(1,1,0,1);
    } 
    else if (maxCoarse == 4)
    {
      out_Color = vec4(0,1,0,1);
    } 
    else
    {
      out_Color = vec4(1,1,1,1);
    }
  }
}

/*
 * Copyright 1993-2018 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */
