#pragma once

#ifndef LYRA_SAMPLES_EDITOR_H
#define LYRA_SAMPLES_EDITOR_H

#include <Lyra/Lyra.hpp>

using namespace lyra;

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

    void resize(const Window& window);

private:
    AppDescriptor desc;

    // logging
    lyra::Logger logger;

    // RHI related items
    OwnedResource<RHI>      rhi;
    OwnedResource<Compiler> slc;
    GPUDevice               device;
    GPUAdapter              adapter;
    GPUSurface              surface;

    // GUI related items
    OwnedResource<GUI> gui;
};

#endif // LYRA_SAMPLES_EDITOR_H
