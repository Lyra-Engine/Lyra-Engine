# Lyra Engine

**Lyra Engine** is an effort to build a rendering engine from scratch.

NOTE: This project is still a work in progress, some of the features might be defined, but unimplemented.
This is a project develop and maintained by single person. Feedbacks and contributons are welcome, but I
have limited time for this project after work, so please do expect slower response time and development speed.

## Build

**Lyra-Engine** uses [CMake](https://cmake.org/) with [vckpkg](https://vcpkg.io/en/) to
manage the build system and the 3rd party dependencies. To simplify the command line,
we adopt [just](https://just.systems/) as the primary command invoker for a lot of
the commonly used commands.

Prior to build, user must specify environment variable **VCPKG_ROOT**.
User can specify one of the presets from **CMakePresets.json**.
User can also create **CMakeUserPresets.json** to overwrite the default configuration.

```bash
just configure msvc     # use MSVC, recommended on Windows
just configure xcode    # use Xcode, recommended on MacOS
just configure ninja    # use Ninja multi-config
```

Once the configure command is invoked, vcpkg will automatically install the required
dependencies defined by **vcpkg.json**. Some of the dependencies might take a while
to compile.

To build the project, users can use the following command:

```bash
just build msvc-debug     # MSVC Debug build
just build msvc-release   # MSVC Release build
just build xcode-debug
just build xcode-release
just build ninja-debug
just build ninja-release
```

## Test

To keep the development process robust, users can build the `testkit` target.
This target is a custom target to launch the test script using Python3. This
script will call the testkit binary in order and compose an HTML page for result.
Currently there is no automatic checking for graphics result, so manual check is
still required.

```bash
just test msvc-debug
just test msvc-release
just test xcode-debug
just test xcode-release
just test ninja-debug
just test ninja-release
```

## Samples

**Lyra-Engine** plans to include both editor and player samples in this repository.
To run the editor/player, user can run the following command:

```bash
just run msvc-debug    editor
just run msvc-release  editor
just run xcode-debug   editor
just run xcode-release editor
just run ninja-debug   editor
just run ninja-release editor
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
* Samples: Primary editor/player applications for this engine.

**Lyra Engine**'s design is largely inspired by **The Machinery** (currently removed from internet).
We adopt a plugin-based design philosophy so that the underlying implementation could be swapped
out with ease. For example, **Library/Render/RHI/API.h** defines a set of API for rendering,
**Plugins/Vulkan" implements this rendering API. However, we could also implement other graphics
APIs like D3D12 and swap out Vulkan with a single line of change.

However, **Lyra Engine**'s plugin APIs do not strictly follow C ABI, meaning that plugins for
non-compliant APIs must be compiled using the same C++ compiler, otherwise we run at the risk
of mis-interpreting C++ structure. The reason for not directly using C ABI is due to the
complexity of the API. For example, some graphics APIs would require multi-level descriptor
objects, and the descriptor would store a vector of objects at some level. Since the user-level
descriptor follows C++ interface, it would require extra work to transform the descriptor into
C ABI form. Given that **Lyra Engine** is a small project that users would often compile the
project as a whole, this wouldn't be a prominent issue.

While **Lyra Engine** is not explicitly requiring C ABI compliant. The only exception being
the modding system. If user mods should be supported, the modding API must strictly follow
C ABI for maximum DLL portability.

## Author(s)

[Tianyu Cheng](tianyu.cheng@utexas.edu)
