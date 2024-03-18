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

#include "GLDemo.h"

#include <glm/glm.hpp>
#include "common.h"
#include "VRSPipeline.h"

#include <cstdint>
#include <memory>

class VRSDemo : public GLDemo< VRSPipeline >
{
public:
    bool begin() override;
    void end() override;

    void renderFrame(double time, uint32_t width, uint32_t height, GLuint fbo) override;

private:
    void processUI(double time) override;
    void updatePerFrameUniforms(uint32_t width, uint32_t height);
    void updateTextures(uint32_t width, uint32_t height);
    void createFoveationTexture(float centerX, float centerY);
    void createConstantFoveationTexture(uint8_t value);
    void uploadFoveationDataToTexture(GLuint texture);
    void setupShadingRatePalette();
    void bindShadingRateTexture();

    uint32_t m_shadingRateImageWidth = 0;
    uint32_t m_shadingRateImageHeight = 0;

    static const int SHADING_MODE_COUNT = 4;
    const char* SHADING_MODE_NAMES[SHADING_MODE_COUNT] = { "Varying shading rate", "1x1 rate", "2x2 rate", "4x4 rate" };
    static const int SHADING_MODE_VARYING = 0;
    static const int SHADING_MODE_1X1 = 1;
    static const int SHADING_MODE_2X2 = 2;
    static const int SHADING_MODE_4X4 = 3;

    GLint m_shadingRateImageTexelWidth;
    GLint m_shadingRateImageTexelHeight;

    GLuint m_shadingRateImageVarying = 0;
    GLuint m_shadingRateImageMouseTracking = 0;
    GLuint m_shadingRateImage1X1 = 0;
    GLuint m_shadingRateImage2X2 = 0;
    GLuint m_shadingRateImage4X4 = 0;

    std::vector<uint8_t> m_shadingRateImageData;

    int m_selectedShadingMode = 0;
    bool m_activateShadingRate = true;
    bool m_visualizeShadingRate = false;
    bool m_fullShadingRateForGreenObjects = true;
};
