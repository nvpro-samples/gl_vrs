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

#ifdef WIN32
#include <accctrl.h>
#include <aclapi.h>
#endif
#include <array>
#include <string>
#include <chrono>
#include <vector>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION

#include "nvh/cameramanipulator.hpp"
#include "backends/imgui_impl_glfw.h"
#include "imgui.h"
#include "nvgl/contextwindow_gl.hpp"
#include "nvgl/extensions_gl.hpp"
#include "nvpsystem.hpp"
#include "stb_image.h"

#include "VRSDemo.h"

int const SAMPLE_SIZE_WIDTH  = 1200;
int const SAMPLE_SIZE_HEIGHT = 900;

// Default search path for shaders
std::vector<std::string> defaultSearchPaths{
    "./",
    "../",
    std::string(PROJECT_NAME),
    std::string("SPV_" PROJECT_NAME),
    NVPSystem::exePath() + PROJECT_RELDIRECTORY,
    NVPSystem::exePath() + std::string(PROJECT_RELDIRECTORY),
    NVPSystem::exePath() + std::string(PROJECT_RELDIRECTORY) + "shaders",
};

//--------------------------------------------------------------------------------------------------
//
//
int main(int argc, const char** argv)
{
  // setup some basic things for the sample, logging file for example
  NVPSystem system(PROJECT_NAME);

  VRSDemo sample;
  return sample.run(PROJECT_NAME, argc, argv, SAMPLE_SIZE_WIDTH, SAMPLE_SIZE_HEIGHT);
}
