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

#include "nvh/geometry.hpp"
#include <glm/glm.hpp>
#include "nvgl/base_gl.hpp"

#include <cstdint>

class Torus
{
public:
    Torus();
    ~Torus();

    // sets buffer state, call draw explicitly (reduce redundant state changes if 
    // multiple objects should be drawn)
    void setBufferState();

    // just unset, won't restore the state from before setBufferState()!
    void unsetBufferState();

    // just the draw calls, use this 
    void draw();

    // values for n,m below 3 will be set to 3.
    void setTessellation(uint32_t n, uint32_t m, float innerRadius = 0.8f, float outerRadius = 0.2f);

    uint32_t getTessellationN() { return m_tessellationN; }
    uint32_t getTessellationM() { return m_tessellationM; }

    void setVertexAttributeLocations(GLuint position, GLuint normal);

    GLsizei getTriangleCount() { return m_numIndices / 3; }

private:
    void regenerateGeometry();

    uint32_t m_tessellationN = 8;
    uint32_t m_tessellationM = 8;
    float m_innerRadius = 0.8f;
    float m_outerRadius = 0.2f;

    struct Vertex {
        Vertex(const nvh::geometry::Vertex& vertex) {
            position = vertex.position;
            normal = vertex.normal;
            color = glm::vec4(1.0f);
        }

        glm::vec4 position;
        glm::vec4 normal;
        glm::vec4 color;
    };

    GLsizei m_numVertices = 0;
    GLsizei m_numIndices = 0;

    bool    m_dataIsUploadedToGPU = false;
    GLuint  m_vbo = 0;
    GLuint  m_ibo = 0;

    GLuint  m_vertexAttributePosition = 0;
    GLuint  m_vertexAttributeNormal = 1;
};
