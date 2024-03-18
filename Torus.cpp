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

#include "Torus.h"

#include <glm/glm.hpp>
#include <vector>

Torus::Torus()
{
}

Torus::~Torus()
{
    nvgl::deleteBuffer(m_vbo);
    nvgl::deleteBuffer(m_ibo);
}

void Torus::setBufferState()
{
    if (!m_dataIsUploadedToGPU)
    {
        regenerateGeometry();
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(m_vertexAttributePosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glVertexAttribPointer(m_vertexAttributeNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)(m_numVertices * 3 * sizeof(float)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    glEnableVertexAttribArray(m_vertexAttributePosition);
    glEnableVertexAttribArray(m_vertexAttributeNormal);
}

void Torus::unsetBufferState()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(m_vertexAttributePosition);
    glDisableVertexAttribArray(m_vertexAttributeNormal);
}

void Torus::draw()
{
    glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, NV_BUFFER_OFFSET(0));
}

void Torus::setTessellation(uint32_t n, uint32_t m, float innerRadius, float outerRadius)
{
    const uint32_t MIN_TES = 3;
    if (n < MIN_TES) n = MIN_TES;
    if (m < MIN_TES) m = MIN_TES;
    if (innerRadius < 0.0f) innerRadius = 0.0f;

    if (   n == m_tessellationN 
        && m == m_tessellationM
        && innerRadius == m_innerRadius
        && outerRadius == m_outerRadius)
    {
        return;
    }
    m_tessellationN = n;
    m_tessellationM = m;
    m_innerRadius = innerRadius;
    m_outerRadius = outerRadius;

    m_dataIsUploadedToGPU = false;
}

void Torus::setVertexAttributeLocations(GLuint position, GLuint normal)
{
    if (position == m_vertexAttributePosition && normal == m_vertexAttributeNormal)
    {
        return;
    }

    m_vertexAttributePosition = position;
    m_vertexAttributeNormal = normal;

    m_dataIsUploadedToGPU = false;
}

void Torus::regenerateGeometry()
{
    uint32_t n = m_tessellationN;
    uint32_t m = m_tessellationM;

    std::vector< glm::vec3 > vertices;
    std::vector< glm::vec3 > tangents;
    std::vector< glm::vec3 > binormals;
    std::vector< glm::vec3 > normals;
    std::vector< glm::vec2 > texcoords;
    std::vector<unsigned int> indices;

    unsigned int size_v = (m + 1) * (n + 1);

    vertices.reserve(size_v);
    tangents.reserve(size_v);
    binormals.reserve(size_v);
    normals.reserve(size_v);
    texcoords.reserve(size_v);
    indices.reserve(6 * m * n);

    float mf = (float)m;
    float nf = (float)n;

    float phi_step = 2.0f * glm::pi<float>() / mf;
    float theta_step = 2.0f * glm::pi<float>() / nf;

    // Setup vertices and normals
    // Generate the Torus exactly like the sphere with rings around the origin along the latitudes.
    for (unsigned int latitude = 0; latitude <= n; latitude++) // theta angle
    {
        float theta = (float)latitude * theta_step;
        float sinTheta = sinf(theta);
        float cosTheta = cosf(theta);

        float radius = m_innerRadius + m_outerRadius * cosTheta;

        for (unsigned int longitude = 0; longitude <= m; longitude++) // phi angle
        {
            float phi = (float)longitude * phi_step;
            float sinPhi = sinf(phi);
            float cosPhi = cosf(phi);

            vertices.push_back(glm::vec3(radius * cosPhi,
                m_outerRadius * sinTheta,
                radius * -sinPhi));

            tangents.push_back(glm::vec3(-sinPhi, 0.0f, -cosPhi));

            binormals.push_back(glm::vec3(cosPhi * -sinTheta,
                cosTheta,
                sinPhi * sinTheta));

            normals.push_back(glm::vec3(cosPhi * cosTheta,
                sinTheta,
                -sinPhi * cosTheta));

            texcoords.push_back(glm::vec2((float)longitude / mf, (float)latitude / nf));
        }
    }

    const unsigned int columns = m + 1;

    // Setup indices
    for (unsigned int latitude = 0; latitude < n; latitude++)
    {
        for (unsigned int longitude = 0; longitude < m; longitude++)
        {
            // two triangles
            indices.push_back(latitude * columns + longitude);  // lower left
            indices.push_back(latitude * columns + longitude + 1);  // lower right
            indices.push_back((latitude + 1) * columns + longitude);  // upper left

            indices.push_back((latitude + 1) * columns + longitude);  // upper left
            indices.push_back(latitude * columns + longitude + 1);  // lower right
            indices.push_back((latitude + 1) * columns + longitude + 1);  // upper right
        }
    }

    m_numVertices = static_cast<GLsizei>(vertices.size());
    GLsizeiptr const sizePositionAttributeData = vertices.size() * sizeof(vertices[0]);
    GLsizeiptr const sizeNormalAttributeData = normals.size() * sizeof(normals[0]);

    m_numIndices = static_cast<GLsizei>(indices.size());
    GLsizeiptr sizeIndexData = indices.size() * sizeof(indices[0]);

    nvgl::newBuffer(m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizePositionAttributeData + sizeNormalAttributeData, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizePositionAttributeData, &vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, sizePositionAttributeData, sizeNormalAttributeData, &normals[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    nvgl::newBuffer(m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndexData, &indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(m_vertexAttributePosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glVertexAttribPointer(m_vertexAttributeNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)(sizePositionAttributeData));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    glEnableVertexAttribArray(m_vertexAttributePosition);
    glEnableVertexAttribArray(m_vertexAttributeNormal);

    m_dataIsUploadedToGPU = true;
}
