#ifndef LYRA_LIBRARY_SCENES_LIGHT_H
#define LYRA_LIBRARY_SCENES_LIGHT_H

#include <Common/GLM.h>

// reference: `https://google.github.io/filament/Filament.md.html

namespace lyra::scn
{

    struct PointLight
    {
        glm::vec4 position_falloff; // x, y, z, falloff coefficient
        glm::vec4 color_intensity;  // r, g, b, intensity in watts
        glm::vec4 direction_ies;    // dx, dy, dy, IES profile index
    };

    struct SpotLight
    {
        glm::vec4 position_falloff; // x, y, z, falloff coefficient
        glm::vec4 color_intensity;  // r, g, b, intensity in watts
        glm::vec4 direction_ies;    // dx, dy, dy, IES profile index
        glm::vec2 angle;            // angle scale, angle offset
    };

    struct DirectionalLight
    {
        glm::vec3 direction;       // dx, dy, dy, IES profile index
        glm::vec4 color_intensity; // r, g, b, intensity in candela
    };

} // namespace lyra::scn

#endif // LYRA_LIBRARY_SCENES_LIGHT_H
