#version 450

#extension GL_ARB_shading_language_include : enable
#extension GL_NV_viewport_array2: require
#extension GL_NV_primitive_shading_rate: require


#include "common.h"

// inputs in model space
in layout(location=VERTEX_POS)    vec3 vertex_pos_model;
in layout(location=VERTEX_NORMAL) vec3 normal;

layout(location=OFFSET_LOC) uniform float offset;

// outputs in view space
out Interpolants {
  centroid vec3 model_pos;
  centroid vec3 normal;
  centroid vec3 eyeDir;
  centroid vec3 lightDir;
} OUT;

void main()
{
  // proj space calculations
  vec4 proj_pos = object.modelViewProj * vec4( vertex_pos_model, 1 );
  gl_Position   = proj_pos + vec4(offset, 0, 0, 0);

  gl_Layer = 0;

  // view space calculations
  vec3 pos      = (object.modelView   * vec4(vertex_pos_model,1)).xyz;
  vec3 lightPos = (scene.viewMatrix   * vec4(scene.lightPos_world,1)).xyz;
  OUT.normal    = ( (object.modelViewIT * vec4(normal,0)).xyz );
  OUT.eyeDir    = (scene.eyePos_view - pos);
  OUT.lightDir  = (lightPos - pos);
  OUT.model_pos = vertex_pos_model;

  //////////// ShadingRateSample ////////////
  //
  // Each viewport can have a different shading rate palette,
  // we use this feature here to shade objects at different rates.
  // As we can set this per triangle (via the provoking vertex)
  // we have a lot of flexibility.
  // Here it's demonstrated just by the object color.
  //
  if (scene.fullShadingRateForGreenObjects == 1)
  {
  //gl_Position.x += 0.1;
    if (object.color.g > 0.8 && object.color.r < 0.2 && object.color.b < 0.2)
    {
      gl_ShadingRateNV = 1;
    }
    else
    {
      gl_ShadingRateNV = 0;
    }
  }
  else
  {
    gl_ShadingRateNV = 0;
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