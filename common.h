/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#ifdef __cplusplus
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
#endif

// general behavior defines
// benchmark mode - change render load and print perf data
#define BENCHMARK_MODE 0    // default 0
// measure times of the different stages (currently not mgpu-safe!)
#define DEBUG_MEASURETIME 0 // default 0
// exit after a set time in seconds
#define DEBUG_EXITAFTERTIME 0 // default 0

// scene data defines
#define VERTEX_POS        0
#define VERTEX_NORMAL     1
#define OFFSET_LOC        2

#define UBO_SCENE         1
#define UBO_OBJECT        2

#ifdef __cplusplus
namespace vertexload
{
#endif
  
  struct SceneData
  {
    mat4 viewMatrix;      // view matrix: world->view
    // mat4 projMatrix;      // proj matrix: view ->proj
    // mat4 viewProjMatrix;  // viewproj   : world->proj

    vec3 lightPos_world;  // light position in world space
    float padding_for_c_1;
    
    vec3 eyepos_world;    // eye position in world space
    float padding_for_c_2;
    
    vec3 eyePos_view;     // eye position in view space
    float padding_for_c_3;

    int loadFactor;
    int fragmentLoadFactor;

    int visualizeShadingRate;
    int fullShadingRateForGreenObjects;
  };

  struct OITSceneData
  {
      mat4 viewMatrix;      // view matrix: world->view
      vec3 lightPos_world;  // light position in world space
      float padding_for_c_1;

      vec3 eyepos_world;    // eye position in world space
      float padding_for_c_2;

      vec3 eyePos_view;     // eye position in view space
      float padding_for_c_3;

      int loadFactor;
      int fragmentLoadFactor;

      int visualizeShadingRate;
      int fullShadingRateForGreenObjects;

      int visualizeSampleCount;
      int maxSamplesForBlending;
      int sampleBufferSizeInSamples;
  };

  struct ObjectData
  {
    mat4 model;         // model -> world
    mat4 modelView;     // model -> view
    mat4 modelViewIT;   // model -> view for normals
    mat4 modelViewProj; // model -> proj
    vec3 color;         // model color
  };

#ifdef __cplusplus
}
#endif

#if defined(GL_core_profile) || defined(GL_compatibility_profile) || defined(GL_es_profile)
// prevent this to be used by c++

#if defined(USE_OIT_SCENE_DATA)
layout(std140, binding = UBO_SCENE) uniform sceneBuffer {
    OITSceneData scene;
};
#else
layout(std140,binding=UBO_SCENE) uniform sceneBuffer {
  SceneData scene;
};
#endif // USE_OIT_SCENE_DATA
layout(std140,binding=UBO_OBJECT) uniform objectBuffer {
  ObjectData object;
};

#endif
