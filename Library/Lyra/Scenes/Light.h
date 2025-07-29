#pragma once

#ifndef LYRA_LIBRARY_SCENES_LIGHT_H
#define LYRA_LIBRARY_SCENES_LIGHT_H

#include <Lyra/Common/GLM.h>
#include <Lyra/Common/Stdint.h>

// reference: `https://google.github.io/filament/Filament.md.html

namespace lyra::scn
{

    struct PointLight
    {
        Vec3  position;  // x, y, z (12 bytes)
        float falloff;   // falloff coefficient (4 bytes)
        Vec3  color;     // r, g, b (12 bytes)
        float intensity; // intensity in watts (4 bytes)
        Vec3  direction; // dx, dy, dz (12 bytes)
        uint  profile;   // IES profile index (4 bytes)
    };

    struct SpotLight
    {
        Vec3  position;  // x, y, z (12 bytes)
        float falloff;   // falloff coefficient (4 bytes)
        Vec3  color;     // r, g, b (12 bytes)
        float intensity; // intensity in watts (4 bytes)
        Vec3  direction; // dx, dy, dz
        uint  profile;   // IES profile index
        Vec2  angle;     // angle scale, angle offset
    };

    struct DirectionalLight
    {
        Vec3  direction; // dx, dy, dz
        uint  profile;   // IES profile index
        Vec3  color;     // r, g, b
        float intensity; // intensity in candela
    };

} // namespace lyra::scn

#endif // LYRA_LIBRARY_SCENES_LIGHT_H
