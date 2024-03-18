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

#include "VRSDemo.h"

#include "util_vrs.h"

bool VRSDemo::begin()
{
    if (!GLDemo::begin()) return false;
    m_pipeline = std::make_unique< VRSPipeline >();

    m_torus.setVertexAttributeLocations(VERTEX_POS, VERTEX_NORMAL);
    m_torusTessellationM = m_torus.getTessellationM();
    m_torusTessellationN = m_torus.getTessellationN();

    if (!isVRSExtensionPresent() || !isPerPrimitiveVRSExtensionPresent())
    {
        return false;
    }

    glEnable(GL_SHADING_RATE_IMAGE_PER_PRIMITIVE_NV);

    glGetIntegerv(GL_SHADING_RATE_IMAGE_TEXEL_HEIGHT_NV, &m_shadingRateImageTexelHeight);
    LOGOK("\nGL_SHADING_RATE_IMAGE_TEXEL_HEIGHT_NV = %d\n", m_shadingRateImageTexelHeight);
    glGetIntegerv(GL_SHADING_RATE_IMAGE_TEXEL_WIDTH_NV, &m_shadingRateImageTexelWidth);
    LOGOK("GL_SHADING_RATE_IMAGE_TEXEL_WIDTH_NV = %d\n", m_shadingRateImageTexelWidth);

    setupShadingRatePalette();

    return true;
}

void VRSDemo::end()
{
    nvgl::deleteTexture(m_shadingRateImageVarying);
    nvgl::deleteTexture(m_shadingRateImageMouseTracking);
    nvgl::deleteTexture(m_shadingRateImage1X1);
    nvgl::deleteTexture(m_shadingRateImage2X2);
    nvgl::deleteTexture(m_shadingRateImage4X4);
    GLDemo::end();
}

void VRSDemo::renderFrame(double time, uint32_t width, uint32_t height, GLuint fbo)
{
    updateTextures(width, height);
    bindShadingRateTexture();
    glViewport(0, 0, width, height);
    updatePerFrameUniforms(width, height);
    m_pipeline->setShaderProgram();
    m_pipeline->updateSceneUniforms();
    renderTori(m_numberOfTori);

    glDisable(GL_SHADING_RATE_IMAGE_NV);
}

void VRSDemo::bindShadingRateTexture()
{
    //////////// ShadingRateSample ////////////
    // 
    // setting the shading rate image:
    // 
    
    switch (m_selectedShadingMode)
    {
    case SHADING_MODE_VARYING:
        glBindShadingRateImageNV(m_shadingRateImageVarying);
        break;
    case SHADING_MODE_1X1:
        glBindShadingRateImageNV(m_shadingRateImage1X1);
        break;    
    case SHADING_MODE_2X2:
        glBindShadingRateImageNV(m_shadingRateImage2X2);
        break;
    case SHADING_MODE_4X4:
    default:
        glBindShadingRateImageNV(m_shadingRateImage4X4);
    }

    if (m_activateShadingRate)
    {
        // independent on the set shading rate image and palette all you need to switch
        // back to full shading rate is one enable / disable:
        glEnable(GL_SHADING_RATE_IMAGE_NV);
    }
}

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void VRSDemo::processUI(double time)
{
    GLDemo::processUI(time);

    if (ImGui::Begin("VRS Demo Settings", nullptr))
    {
        ImGui::TextUnformatted("Input manually with CTRL+Click");

        ImGui::SliderInt("Tori", &m_numberOfTori, 1, 1000, "%d", ImGuiSliderFlags_None);
        ImGui::SameLine(); HelpMarker("Input manually with CTRL+Click.");

        ImGui::SliderInt("Fragment load", &m_fragmentLoad, 1, 250, "%d", ImGuiSliderFlags_None);

        ImGui::SliderInt("Torus tessellation N", &m_torusTessellationN, 3, 64, "%d", ImGuiSliderFlags_None);
        ImGui::SliderInt("Torus tessellation M", &m_torusTessellationM, 3, 64, "%d", ImGuiSliderFlags_None);
        ImGui::Text("Triangle count per torus: %d", (int)m_torus.getTriangleCount());

        ImGui::Separator();

        ImGui::ListBox("Shading mode", &m_selectedShadingMode, SHADING_MODE_NAMES, SHADING_MODE_COUNT, SHADING_MODE_COUNT);

        ImGui::Checkbox("Enable VRS", &m_activateShadingRate);
        ImGui::Checkbox("visualize ShadingRate", &m_visualizeShadingRate);
        ImGui::Checkbox("full ShadingRate for green objects", &m_fullShadingRateForGreenObjects);
    }
    ImGui::End();

    if (m_torusTessellationM != m_torus.getTessellationM()
        || m_torusTessellationN != m_torus.getTessellationN())
    {
        m_torus.setTessellation(m_torusTessellationN, m_torusTessellationM);
    }
}

void VRSDemo::updatePerFrameUniforms(uint32_t width, uint32_t height)
{
    auto view = m_control.m_viewMatrix;
    auto iview = glm::inverse(view);

    auto proj = glm::perspective(45.f, float(width) / float(height), 0.01f, 10.0f);


    float depth = 1.0f;
    glm::vec4 background = glm::vec4(118.f / 255.f, 185.f / 255.f, 0.f / 255.f, 0.f / 255.f);
    glm::vec3 eyePos_world = glm::vec3(iview[0][3], iview[1][3], iview[2][3]);
    glm::vec3 eyePos_view = view * glm::vec4(eyePos_world, 1.0f);

    m_pipeline->sceneData.viewMatrix = view;
    //m_pipeline->sceneData.projMatrix = proj;
    //m_pipeline->sceneData.viewProjMatrix = proj * view;
    m_pipeline->sceneData.lightPos_world = eyePos_world;
    //m_pipeline->sceneData.eyepos_world = eyePos_world;
    m_pipeline->sceneData.eyePos_view = eyePos_view;
    m_pipeline->sceneData.loadFactor = m_numberOfTori;
    m_pipeline->sceneData.fragmentLoadFactor = m_fragmentLoad;
    m_pipeline->sceneData.visualizeShadingRate = m_visualizeShadingRate ? 1 : 0;
    m_pipeline->sceneData.fullShadingRateForGreenObjects = m_fullShadingRateForGreenObjects ? 1 : 0;

    m_pipeline->setProjectionMatrix(proj);
    m_pipeline->setViewMatrix(m_control.m_viewMatrix);

    // upload to GPU:
    m_pipeline->updateSceneUniforms();
}

void VRSDemo::updateTextures(uint32_t width, uint32_t height)
{
    uint32_t textureWidth = (width + m_shadingRateImageTexelWidth - 1) / m_shadingRateImageTexelWidth;
    uint32_t textureHeight = (height + m_shadingRateImageTexelHeight - 1) / m_shadingRateImageTexelHeight;

    if (textureWidth == m_shadingRateImageWidth && textureHeight == m_shadingRateImageHeight)
    {
        return;
    }

    m_shadingRateImageWidth = textureWidth;
    m_shadingRateImageHeight = textureHeight;

    m_shadingRateImageData.resize(m_shadingRateImageWidth * m_shadingRateImageHeight);

    nvgl::newTexture(m_shadingRateImageVarying, GL_TEXTURE_2D);
    nvgl::newTexture(m_shadingRateImageMouseTracking, GL_TEXTURE_2D);
    nvgl::newTexture(m_shadingRateImage1X1, GL_TEXTURE_2D);
    nvgl::newTexture(m_shadingRateImage2X2, GL_TEXTURE_2D);
    nvgl::newTexture(m_shadingRateImage4X4, GL_TEXTURE_2D);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //
    // The first shading rate image will have the full resolution
    // in the center and a lower rate towards the edges.
    //
    createFoveationTexture(0.5f, 0.5f);
    uploadFoveationDataToTexture(m_shadingRateImageVarying);

    //
    // The mouse tracking shading rate image will be the same as the varying shading rate 
    // image, but will use to mouse position to determine its 'center'. The actual values 
    // do not matter here, because the shading rate image will be overwritteneach frame. 
    //
    uploadFoveationDataToTexture(m_shadingRateImageMouseTracking);

    //
    // The remaining three images have a constant value. This could also simply be
    // done by different palettes but we wanted to show how to change to a
    // completely different shading rate image here.
    //
    createConstantFoveationTexture(1);
    uploadFoveationDataToTexture(m_shadingRateImage1X1);

    createConstantFoveationTexture(2);
    uploadFoveationDataToTexture(m_shadingRateImage2X2);

    createConstantFoveationTexture(3);
    uploadFoveationDataToTexture(m_shadingRateImage4X4);

    GLenum errorCode = glGetError(); assert(errorCode == GL_NO_ERROR); // verify there are no errors during development

    glBindTexture(GL_TEXTURE_2D, 0);
}

void VRSDemo::createFoveationTexture(float centerX, float centerY)
{
    //////////// ShadingRateSample ////////////
    // 
    // Creates the data for a 'foveation' shading rate imaage. It will have a
    // high resolution at the given center and a lower rate further away from
    // the center. At the edges we also use the SHADE_NO_PIXELS_NV rate
    // which will discard the full block. This is useful for areas in HMDs
    // which won't end up on the screen anyway due to the lens distortions.
    //
    const int width = m_shadingRateImageWidth;
    const int height = m_shadingRateImageHeight;

    for (size_t y = 0; y < height; ++y)
    {
        for (size_t x = 0; x < width; ++x)
        {
            float fx = x / (float)width;
            float fy = y / (float)height;

            float d = std::sqrt((fx - centerX) * (fx - centerX) + (fy - centerY) * (fy - centerY));

            if (d < 0.15f)
            {
                m_shadingRateImageData[x + y * width] = 1;
            }
            else if (d < 0.3f)
            {
                m_shadingRateImageData[x + y * width] = 2;
            }
            else if (d < 0.45f)
            {
                m_shadingRateImageData[x + y * width] = 3;
            }
            else
            {
                m_shadingRateImageData[x + y * width] = 0;
            }
        }
    }
}

void VRSDemo::createConstantFoveationTexture(uint8_t value)
{
    for (size_t y = 0; y < m_shadingRateImageHeight; ++y)
    {
        for (size_t x = 0; x < m_shadingRateImageWidth; ++x)
        {
            m_shadingRateImageData[x + y * m_shadingRateImageWidth] = value;
        }
    }
}

void VRSDemo::uploadFoveationDataToTexture(GLuint texture)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8UI, m_shadingRateImageWidth, m_shadingRateImageHeight);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_shadingRateImageWidth, m_shadingRateImageHeight, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &m_shadingRateImageData[0]);
}

void VRSDemo::setupShadingRatePalette()
{
    GLint palSize;
    glGetIntegerv(GL_SHADING_RATE_IMAGE_PALETTE_SIZE_NV, &palSize);
    LOGOK("GL_SHADING_RATE_IMAGE_PALETTE_SIZE_NV = %d\n\n", palSize);

    //
    // We only set the first four entries explicitly, so check that the
    // GPU supports at least 4 entries. Actual hardware supports more.
    //
    assert(palSize >= 4);

    //////////// ShadingRateSample ////////////
    //
    // setting the palettes
    // The second palette is used to send geometry in the Vertex Shader 
    // to an alternative shading rate.
    //
    GLenum* palette = new GLenum[palSize];

    palette[0] = GL_SHADING_RATE_NO_INVOCATIONS_NV;
    palette[1] = GL_SHADING_RATE_1_INVOCATION_PER_PIXEL_NV;
    palette[2] = GL_SHADING_RATE_1_INVOCATION_PER_2X2_PIXELS_NV;
    palette[3] = GL_SHADING_RATE_1_INVOCATION_PER_4X4_PIXELS_NV;
    // fill the rest
    for (size_t i = 4; i < palSize; ++i)
    {
        palette[i] = GL_SHADING_RATE_1_INVOCATION_PER_PIXEL_NV;
    }

    glShadingRateImagePaletteNV(0, 0, palSize, palette);
    delete[] palette;

    //
    // One palette with a constant rate:
    //
    GLenum* paletteFullRate = new GLenum[palSize];
    for (size_t i = 0; i < palSize; ++i)
    {
        paletteFullRate[i] = GL_SHADING_RATE_1_INVOCATION_PER_PIXEL_NV;
    }

    glShadingRateImagePaletteNV(1, 0, palSize, paletteFullRate);
    delete[] paletteFullRate;
}
