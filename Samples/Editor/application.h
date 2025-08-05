#pragma once

#ifndef LYRA_SAMPLES_EDITOR_H
#define LYRA_SAMPLES_EDITOR_H

#include <Lyra/Common.hpp>
#include <Lyra/Render.hpp>
#include <Lyra/Window.hpp>

using namespace lyra;
using namespace lyra::rhi;
using namespace lyra::wsi;
using namespace lyra::gui;

struct AppDescriptor
{
    WindowHandle  window;
    RHIBackend    rhi_backend;
    RHIFlags      rhi_flags;
    CompileTarget slc_target;
    CompileFlags  slc_flags;
};

struct Application
{
    explicit Application(const AppDescriptor& desc) : desc(desc) {}

    void init();

    void destroy();

    void update(const Window& window);

    void render();

    void resize();

private:
    AppDescriptor desc;

    // logging
    lyra::Logger logger;

    // RHI related stuff
    OwnedResource<RHI>      rhi;
    OwnedResource<Compiler> slc;
    GPUDevice               device;
    GPUAdapter              adapter;
    GPUSurface              surface;

    // rendering related stuff
    OwnedResource<GUIRenderer> gui;
};

#endif // LYRA_SAMPLES_EDITOR_H
