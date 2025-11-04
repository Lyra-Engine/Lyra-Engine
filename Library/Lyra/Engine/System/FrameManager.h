#pragma once

#ifndef LYRA_LIBRARY_ENGINE_SYSTEM_FRAME_MANAGER_H
#define LYRA_LIBRARY_ENGINE_SYSTEM_FRAME_MANAGER_H

#include <Lyra/Vendor/IMGUI.h>
#include <Lyra/AppKit/AppTypes.h>
#include <Lyra/Render/RHI/RHITypes.h>

namespace lyra
{
    struct ImGuiFrame
    {
        ImTextureID texture    = static_cast<ImTextureID>(0);
        GPUTexture  backbuffer = GPUTexture();
    };

    struct FrameManager
    {
    public:
        explicit FrameManager(uint frames_in_flight);

        void bind(Application& app);

        void update(Blackboard& blackboard);

    private:
        void create_frames();
        void delete_frames();

    private:
        uint               frames_in_flight;
        Vector<ImGuiFrame> frames;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_ENGINE_SYSTEM_FRAME_MANAGER_H
