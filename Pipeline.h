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

#include <glm/glm.hpp>
#include "nvgl/programmanager_gl.hpp"
#include "nvgl/base_gl.hpp"

extern std::vector<std::string> defaultSearchPaths;

template <class SCENE_DATA, class OBJECT_DATA>
class Pipeline
{
public:
    Pipeline(GLuint sceneBufferIndex, GLuint objectBufferIndex)
        : m_sceneBufferIndex(sceneBufferIndex)
        , m_objectBufferIndex(objectBufferIndex)
    {
        nvgl::newBuffer(m_sceneUbo);
        glNamedBufferData(m_sceneUbo, sizeof(SCENE_DATA), nullptr, GL_DYNAMIC_DRAW);
        
        nvgl::newBuffer(m_objectUbo);
        glNamedBufferData(m_objectUbo, sizeof(OBJECT_DATA), nullptr, GL_DYNAMIC_DRAW);

        for (const auto& path : defaultSearchPaths)
        {
            m_progManager.addDirectory(path);
        }
    };
    virtual ~Pipeline() {
        m_progManager.deletePrograms();
        nvgl::deleteBuffer(m_sceneUbo);
        nvgl::deleteBuffer(m_objectUbo);
    };

    void setModelMatrix(const glm::mat4& modelMatrix)
    {
        m_modelMatrix = modelMatrix;
    }

    void setViewMatrix(const glm::mat4& viewMatrix)
    {
        m_viewMatrix = viewMatrix;
    }

    void setProjectionMatrix(const glm::mat4& viewMatrix)
    {
        m_projectionMatrix = viewMatrix;
    }

    void reloadShaders()
    {
        m_progManager.reloadPrograms();
    }

    virtual void setShaderProgram()
    {
        glUseProgram(m_progManager.get(m_program));
    }
    virtual void updateSceneUniforms();
    virtual void updateObjectUniforms();

    SCENE_DATA sceneData;
    OBJECT_DATA objectData;

protected:
    glm::mat4 m_modelMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    nvgl::ProgramManager m_progManager;

    GLuint m_objectUbo = 0;
    GLuint m_sceneUbo = 0;
    GLuint m_sceneBufferIndex = 0;
    GLuint m_objectBufferIndex = 1;

    nvgl::ProgramID m_program;
};

template<class SCENE_DATA, class OBJECT_DATA>
inline void Pipeline<SCENE_DATA, OBJECT_DATA>::updateSceneUniforms()
{
    glNamedBufferSubData(m_sceneUbo, 0, sizeof(SCENE_DATA), &sceneData);
    glBindBufferBase(GL_UNIFORM_BUFFER, m_sceneBufferIndex, m_sceneUbo);
}

template<class SCENE_DATA, class OBJECT_DATA>
inline void Pipeline<SCENE_DATA, OBJECT_DATA>::updateObjectUniforms()
{
    objectData.model = m_modelMatrix;
    objectData.modelView = m_viewMatrix * m_modelMatrix;
    objectData.modelViewIT = glm::transpose(glm::inverse(objectData.modelView));
    objectData.modelViewProj = m_projectionMatrix * m_viewMatrix * m_modelMatrix;

    glNamedBufferSubData(m_objectUbo, 0, sizeof(OBJECT_DATA), &objectData);

    glBindBufferBase(GL_UNIFORM_BUFFER, m_objectBufferIndex, m_objectUbo);
}
