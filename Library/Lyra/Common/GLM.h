#pragma once

#ifndef LYRA_LIBRARY_COMMON_GLM_H
#define LYRA_LIBRARY_COMMON_GLM_H

// THIS IS PURELY A HEADER WRAPPER FOR GLM, with specific macros defined.

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

// type alias
using Vec2 = glm::mediump_vec2;
using Vec3 = glm::mediump_vec3;
using Vec4 = glm::mediump_vec4;

#endif // LYRA_LIBRARY_COMMON_GLM_H
