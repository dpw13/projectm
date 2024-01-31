/**
 * @file render_opengl.h
 * @copyright 2003-2023 projectM Team
 * @brief Functions to configure and render projectM visuals using OpenGL.
 *
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2023 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */

#pragma once

#include <GL/glew.h>
#include "projectM-4/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Renders a single frame.
 *
 * @note Separate two-pass frame rendering is currently not supported by the C API as it is rarely used
 *       and also depends on the loaded preset.
 *
 * @param instance The projectM instance handle.
 */
PROJECTM_EXPORT void projectm_opengl_render_frame(projectm_handle instance);

/**
 * @brief Creates and returns the GL name (ID) of a texture that holds a copy of
 * the output frame.
 * 
 * The size will always match the size of the window and is updated if the window
 * size is changed.
 * 
 * @param instance The projectM instance handle.
 * @param internalFormat The number of color components in the texture, e.g. GL_RGB, GL_RGBA4.
 * @param format Specifies the format of the pixel data, e.g. GL_RED, GL_BGRA.
 * @param type Specifies the data type of the pixel data, e.g. GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_INT_8_8_8_8.
 * @return The ID of the new texture.
 */
PROJECTM_EXPORT GLint projectm_opengl_get_output_texture(projectm_handle instance, GLint internalFormat, GLenum format, GLenum type);

#ifdef __cplusplus
} // extern "C"
#endif
