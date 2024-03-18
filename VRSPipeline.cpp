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

#include "VRSPipeline.h"

#include "nvh/nvprint.hpp"

VRSPipeline::VRSPipeline()
    : Pipeline< vertexload::SceneData, vertexload::ObjectData >(UBO_SCENE, UBO_OBJECT)
{
    m_progManager.registerInclude("common.h", "common.h");
    m_progManager.registerInclude("noise.glsl", "noise.glsl");

    m_program = m_progManager.createProgram(
        nvgl::ProgramManager::Definition(GL_VERTEX_SHADER, "#define USE_VIEWPORT\n", "scene.vert.glsl"),
        nvgl::ProgramManager::Definition(GL_FRAGMENT_SHADER, "", "scene.frag.glsl"));

    bool valid = m_progManager.areProgramsValid();
    if (!valid)
    {
        LOGE("Error loading shader files\n");
    }
}

VRSPipeline::~VRSPipeline()
{
}

void VRSPipeline::updateObjectUniforms()
{
    objectData.color = m_objectColor;

    Pipeline< vertexload::SceneData, vertexload::ObjectData >::updateObjectUniforms();
}

