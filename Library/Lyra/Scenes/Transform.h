#ifndef LYRA_LIBRARY_SCENES_TRANSFORM_H
#define LYRA_LIBRARY_SCENES_TRANSFORM_H

#include <Lyra/Common/GLM.h>

namespace lyra::scn
{

    struct Transform
    {
        glm::vec3 scale    = glm::vec3(1.0f);
        glm::vec3 position = glm::vec3(0.0f);
        glm::quat rotation = glm::quat();
    };

    struct WorldTransform
    {
        glm::mat4 xform = glm::mat4();

        bool dirty = false;
    };

    struct CameraTransform
    {
        glm::mat4 projection;
    };

} // namespace lyra::scn

#endif // LYRA_LIBRARY_SCENES_TRANSFORM_H
