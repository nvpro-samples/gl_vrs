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

#include "util_vrs.h"

#include "nvgl/base_gl.hpp"
#include "nvh/nvprint.hpp"

#include <string>

bool isExtensionPresent(const char *extName)
{
    GLint numExtensions;
    bool extFound = false;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    for (GLint i = 0; i < numExtensions && !extFound; ++i)
    {
        std::string name((const char*)glGetStringi(GL_EXTENSIONS, i));
        extFound = (name == extName);
    }

    if (!extFound)
    {
        LOGE("\n%s extension NOT found!\n\n", extName);
    }

    return extFound;
}

bool isVRSExtensionPresent()
{
    return isExtensionPresent("GL_NV_shading_rate_image");
}

bool isPerPrimitiveVRSExtensionPresent()
{
    return isExtensionPresent("GL_NV_primitive_shading_rate");
}
