#ifndef LYRA_LIBRARY_SCENES_CAMERA_H
#define LYRA_LIBRARY_SCENES_CAMERA_H

#include <Scenes/Transform.h>

namespace lyra
{

    struct Camera
    {
        Transform proj;
        Transform view;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_SCENES_CAMERA_H
