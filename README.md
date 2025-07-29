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

## Build

Prior to build, user must specify environment variable **VCPKG_ROOT**.
User can specify one of the presets from **CMakePresets.json**.
User can also overwrite the default generator (for example, switching to **Ninja**).

```bash
cmake -S . -B Scratch --preset Debug             # -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake -S . -B Scratch --preset Release           # -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake -S . -B Scratch --preset RelWithDebInfo    # -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake -S . -B Scratch -A x64 --preset MSVC       # -G "Visual Studio 17"
```

After the project solution is created, users can build the project using:

```bash
cmake --build Scratch -- -j 8          # enable multi-thread compiling for Unix Makefiles
cmake --build Scratch --config Release # select one of the configs if using MSVC
```

## Test

To keep the development process robust, users can build the `testkit` target.
This target is a custom target to launch the test script using Python3. This
script will call the test kit binary in order and compose an HTML page for result.
Currently there is no automatic checking, so manual check is still required.

```bash
cmake --build Scratch --target testkit # run tests and show HTML report
```

## Install

User can also install the built (release) binary into system directory.
Once installed, other projects can use `find_pacakge(Lyra-Engine)` and
use it from other projects.

```bash
cmake --install Scratch  # copy the built libraries and headers into system directory
```

## Components

**Lyra Engine** consists of the following parts:

* Library: The core shared library for the rendering engine (to be linked by user application).
* Plugins: Shared libraries loaded at runtime by library (not directly linked).
* Samples: Basic example as guide on how to use this engine (Permanently moved to [Lyra-Samples](https://github.com/Lyra-Engine/Lyra-Samples)).

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

## Author(s)

[Tianyu Cheng](tianyu.cheng@utexas.edu)
