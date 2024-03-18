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

#include "nvgl/appwindowprofiler_gl.hpp"
#include "nvgl/base_gl.hpp"
#include "nvgl/error_gl.hpp"
#include "nvgl/extensions_gl.hpp"
#include "nvh/cameracontrol.hpp"
#include <glm/glm.hpp>

#include "imgui/backends/imgui_impl_gl.h"
#include "imgui/imgui_helper.h"

#include "Pipeline.h"
#include "Torus.h"

#include <memory>

template <class PIPELINE>
class GLDemo : public nvgl::AppWindowProfilerGL
{
public:
    GLDemo() = default;

    bool begin() override;
    void think(double time) final;
    void end() override;
    void resize(int width, int height) final;

    bool mouse_pos(int x, int y) override { return ImGuiH::mouse_pos(x, y); }
    bool mouse_button(int button, int action) override { return ImGuiH::mouse_button(button, action); }
    bool mouse_wheel(int wheel) override { return ImGuiH::mouse_wheel(wheel); }
    bool key_char(int button) override { return ImGuiH::key_char(button); }
    bool key_button(int button, int action, int mods) override { return ImGuiH::key_button(button, action, mods); }

    virtual void renderFrame(double time, uint32_t width, uint32_t height, GLuint fbo) = 0;

protected:
    // called from think:
    virtual void processUI(double time);
    virtual void reloadShaders() {};
    nvh::CameraControl m_control;

    std::unique_ptr< PIPELINE > m_pipeline = nullptr;

    // torus related:
    void renderTori(uint32_t numberOfTori);
    Torus m_torus;
    int m_torusTessellationN;
    int m_torusTessellationM;
    int m_numberOfTori = 16;
    int m_fragmentLoad = 16;

private:
    void clearFrameBuffer();
    void blitFrameBufferToScreen();

    double m_uiTime = 0.0;

    // init and resize:
    bool initFramebuffers(int width, int height);
    void initCameraControl();

    // OpenGL objects:
    struct
    {
        GLuint scene_color = 0;
        GLuint scene_depthstencil = 0;
    } m_textures;

    GLuint m_fbo = 0;

    int getWindowWidth() {
        return m_windowState.m_winSize[0];
    }
    int getWindowHeight() {
        return m_windowState.m_winSize[1];
    }
    int getFramebufferWidth() {
        return m_windowState.m_winSize[0] / m_framebufferScaling;
    }
    int getFramebufferHeight() {
        return m_windowState.m_winSize[1] / m_framebufferScaling;
    }
    int m_framebufferScaling = 1;
};


extern std::vector<std::string> defaultSearchPaths;

template <class PIPELINE>
bool GLDemo<PIPELINE>::begin()
{
    ImGuiH::Init(getWindowWidth(), getWindowHeight(), this);
    ImGui::InitGL();

    initCameraControl();

    bool initOK = true;
    initOK &= initFramebuffers(getWindowWidth(), getWindowHeight());

    return initOK;
}

template <class PIPELINE>
void GLDemo<PIPELINE>::think(double time)
{
    ImGui::NewFrame();
    processUI(time);

    m_control.processActions({m_windowState.m_winSize[0],m_windowState.m_winSize[1]},
        glm::vec2(m_windowState.m_mouseCurrent[0], m_windowState.m_mouseCurrent[1]),
        m_windowState.m_mouseButtonFlags, m_windowState.m_mouseWheel);

    clearFrameBuffer();

    renderFrame(time, getFramebufferWidth(), getFramebufferHeight(), m_fbo);

    blitFrameBufferToScreen();

    ImGui::Render();
    ImGui::RenderDrawDataGL(ImGui::GetDrawData());
    ImGui::EndFrame();
}

template <class PIPELINE>
void GLDemo<PIPELINE>::end()
{
    ImGui::ShutdownGL();
}

template <class PIPELINE>
void GLDemo<PIPELINE>::resize(int width, int height)
{
    initFramebuffers(width, height);
}

template <class PIPELINE>
void GLDemo<PIPELINE>::processUI(double time)
{
    // Update imgui configuration
    auto& imgui_io = ImGui::GetIO();
    imgui_io.DeltaTime = static_cast<float>(time - m_uiTime);
    imgui_io.DisplaySize = ImVec2(getWindowWidth(), getWindowHeight());

    m_uiTime = time;

    ImGui::SetNextWindowPos({30, 30}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImGuiH::dpiScaled(450, 0), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("NVIDIA " PROJECT_NAME, nullptr))
    {
        ImGui::SliderInt("Framebuffer scaling", &m_framebufferScaling, 1, 16);

        if (ImGui::Button("Reload Shader"))
        {
            m_pipeline->reloadShaders();
            reloadShaders();
        }
    }
    ImGui::End();
}

template<class PIPELINE>
void GLDemo<PIPELINE>::renderTori(uint32_t numberOfTori)
{
    m_torus.setBufferState();

    float num = (float)numberOfTori;
    int width = m_windowState.m_winSize[0];
    int height = m_windowState.m_winSize[1];

    // distribute num tori into an numX x numY pattern
    // with numX * numY > num, numX = aspect * numY

    float aspect = (float)width / (float)height;

    size_t numX = static_cast<size_t>(ceil(sqrt(num * aspect)));
    size_t numY = static_cast<size_t>((float)numX / aspect);
    if (numX * numY < num)
    {
        ++numY;
    }
    float rx = 1.0f;                     // radius of ring
    float ry = 1.0f;
    float dx = 1.0f;                     // ring distance
    float dy = 1.5f;
    float sx = (numX - 1) * dx + 2 * rx; // array size 
    float sy = (numY - 1) * dy + 2 * ry;

    float x0 = -sx / 2.0f + rx;
    float y0 = -sy / 2.0f + ry;

    float scale = std::min(1.f / sx, 1.f / sy) * 0.8f;

    size_t torusIndex = 0;
    for (size_t i = 0; i < numY && torusIndex < num; ++i)
    {
        for (size_t j = 0; j < numX && torusIndex < num; ++j)
        {
            float y = y0 + i * dy;
            float x = x0 + j * dx;

            float rotationAngle = (j % 2 ? -1.0f : 1.0f) * 45.0f * glm::pi<float>() / 180.0f;
            glm::mat4 modelMatrix =
                glm::scale(glm::mat4(1.0f), glm::vec3(scale))
                * glm::translate(glm::mat4(1.f), glm::vec3(x, y, 0.0f))
                * glm::rotate(glm::mat4(1.f), rotationAngle, glm::vec3(1, 0, 0));

            m_pipeline->setModelMatrix(modelMatrix);

            // Use colors light blue and green
            int colorIndex = torusIndex % 5;
            glm::vec3 color(0, .7f, 1);
            if (colorIndex == 4) {
                color = glm::vec3(0, 1, 0);
            }

            m_pipeline->setObjectColor(color);
            m_pipeline->updateObjectUniforms();

            m_torus.draw();

            ++torusIndex;
        }
    }

    m_torus.unsetBufferState();
}

template <class PIPELINE>
void GLDemo<PIPELINE>::clearFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, getWindowWidth(), getWindowHeight());
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

template <class PIPELINE>
void GLDemo<PIPELINE>::blitFrameBufferToScreen()
{
    // blit to background
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, getFramebufferWidth(), getFramebufferHeight(), 0, 0, getWindowWidth(), getWindowHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

template <class PIPELINE>
bool GLDemo<PIPELINE>::initFramebuffers(int width, int height)
{
    const GLsizei mipLevels = 1;

    nvgl::newTexture(m_textures.scene_color, GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_textures.scene_color);
    glTexStorage2D(GL_TEXTURE_2D, mipLevels, GL_RGBA8, width, height);

    nvgl::newTexture(m_textures.scene_depthstencil, GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_textures.scene_depthstencil);
    glTexStorage2D(GL_TEXTURE_2D, mipLevels, GL_DEPTH24_STENCIL8, width, height);

    nvgl::newFramebuffer(m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textures.scene_color, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_textures.scene_depthstencil, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

template <class PIPELINE>
void GLDemo<PIPELINE>::initCameraControl()
{
    m_control.m_sceneOrbit = glm::vec3(0.0f);
    m_control.m_sceneDimension = 2.0f;
    float dist = m_control.m_sceneDimension * 0.75f;
    m_control.m_viewMatrix = glm::lookAt(
        m_control.m_sceneOrbit - glm::normalize(glm::vec3(1, 0, -1)) * dist,
        m_control.m_sceneOrbit,
        glm::vec3(0, 1, 0));
}
