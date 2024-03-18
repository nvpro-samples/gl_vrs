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

#include "Pipeline.h"

#include <glm/glm.hpp>
#include "common.h"

#include "nvgl/base_gl.hpp"

class VRSPipeline : public Pipeline< vertexload::SceneData, vertexload::ObjectData >
{
public:
    VRSPipeline();
    ~VRSPipeline();

    void setObjectColor(const glm::vec3& color)
    {
        m_objectColor = color;
    }

    void updateObjectUniforms() override;

private:
    glm::vec3 m_objectColor;
};
