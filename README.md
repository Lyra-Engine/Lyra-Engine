# Lyra Engine

**Lyra Engine** is an effort to build a rendering engine from scratch.

NOTE: This project is still a work in progress, some of the features might be defined, but unimplemented.
This is a project develop and maintained by single person. Feedbacks and contributons are welcome, but I
have limited time for this project after work, so please do expect slower response time and development speed.

## Dependencies

**Lyra Engine** is a CMake-based project. While CMake provides a comprehensive build system,
it does not provide a mature package repository. Therefore, I resort to **vcpkg** for help.
For well-known dependencies used in this projects, e.g. glfw, the build system will first
search for existing installation from **vcpkg**. However, as a fallback, **Lyra Engine**
will use CMake's FetchContent mechanism to build external packages as project dependency.

Majority of the dependencies are built in the way described above. However, there are a
number of dependencies would soly rely on **vcpkg**. This includes **usd**, **shader-slang**,
etc. These dependencies are large and slow to build (and would often fail to build on various
development environment). Users would need to manually install them using **vcpkg**.

## Build & Install

Prior to build, user must specify environment variable **VCPKG_ROOT**.
User can specify one of the presets from **CMakePresets.json**.
User can also overwrite the default generator (for example, switching to **Ninja**).

```bash
cmake -S . -B Scratch -A x64 --preset Debug             # -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake -S . -B Scratch -A x64 --preset Release           # -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake -S . -B Scratch -A x64 --preset RelWithDebInfo    # -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake -S . -B Scratch -A x64 --preset MSVC              # -G "Visual Studio 17"
```

After the project solution is created, users can build the project using:

```bash
cd Scratch                       # cmake --build command must be invoked in project solution directory
cmake --build . -- -j 8          # enable multi-thread compiling for Unix Makefiles
cmake --build . --config Release # select one of the configs if using MSVC
```

User can also install the built (release) binary into system directory:

```bash
cd Scratch                       # cmake --install command must be invoked in project solution directory
cmake --install .                # copy the built libraries and headers into system directory
```

## Components

**Lyra Engine** consists of the following parts:

* Library: The core shared library for the rendering engine (to be linked by user application).
* Plugins: Shared libraries loaded at runtime by library (not directly linked).
* Samples: Basic example as guide on how to use this engine.

**Lyra Engine**'s design is largely inspired by **The Machinery** (currently removed from internet).
We adopt a plugin-based design philosophy so that the underlying implementation could be swapped
out with ease. For example, **Library/Render/RHI/API.h** defines a set of API for rendering,
**Plugins/Vulkan" implements this rendering API. However, we could also implement other graphics
APIs like D3D12 and swap out Vulkan with a single line of change.

However, **Lyra Engine**'s plugin APIs do not strictly follow C ABI, meaning that plugins for
non-compliant APIs must be compiled using the same C++ compiler, otherwise we run at the risk
of mis-interpreting C++ structure. The reason for not directly using C ABI is due to the
complexity of the API. Some graphics APIs would require multi-level descriptor objects, and
the descriptor would store a vector of objects at some level. Since the user-level descriptor
follows C++ interface, it would require extra work to transform the descriptor into C ABI form.
Given that **Lyra Engine** is a small project that users would often compile the project as a whole,
this wouldn't be a big issue.

## Graphics API

In the past I have made multiple attempts to design my own abstraction layer for graphics API.
My personal experience mostly focused on Vulkan (which I think is the most verbose API by far),
which made my abstraction layer more or less like Vulkan (because it is easy). However, this
might cause implementation for other graphics APIs hard. Therefore **Lyra Engine** chooses to
follow the API design of WebGPU. WebGPU has a good selection of subset of all major APIs, and
has proven to be powerful. Additionally, projects like **wgpu** or **dawn** has implemented
WebGPU using other APIs before, so this path is also proven.

However, **Lyra Engine** does not strictly follow WebGPU, because it is still an implicit API
that tracks resource lifetimes and ownerships. This would introduce slight overhead, while our
goal is to make the overhead as low as possible. With FrameGraph / RenderGraph being the de facto
standard practice of 3A game engines now, it would be best to design an explicit API, but use
render graph to track resource usages, combining the best of two worlds.

Additionally, **Lyra Engine** does not follow the Metal style command encoder design. Although
separating the command recording into encoders might be a clear design, but in practice it is
a little too tedious to create different encoder objects while recording GPU commands. Instead,
we fallback to D3D12 and Vulkan's command list style command recording.

WebGPU does not support ray tracing APIs yet. For this part, we are partly borrowing design from
Rust **wgpu** and Vulkan. This part is still under design phase.

## Shading Language

Our choice of shading language is **slang**. It is mostly compatible with HLSL, while introducing
more features such as modules, generics, etc to facilitate shader development. A good number of
projects have chosen **slang** for research or production. However, this being said, users can
implement their own choice of shading language following the APIs defined in **Library/SLC/API.h**.

We also plan to implement the shader reflection API to alleviate the manual pipeline layout creation.
The shader reflection API design is still in progress.

## Scene Management

Our choice of scene management design is based on ECS. Historically I have relied on hierarchical
scene graph, but this design would gradually couple more operations into the scene node class as
the system gets more complicated. Therefore, we adopted ECS based scene management system to decouple
the scene node entity from all kinds of logic. Components can be added or removed independently
without modifying the existing node class. An implicit scene node hierarchy is still preserved
by adding **ParentNode** component, which records the parent entity.

However, the ECS-based scene management does not naturally support walking the scene hierarchy
like walking down a tree. Common operations such as frustum culling might be not as efficient.
Therefore we might want to implement an OctTree as well for culling purpose.

## Material System

Not started yet.

## Asset Management

Not started yet.

## Author(s)

[Tianyu Cheng](tianyu.cheng@utexas.edu)
