# Render Library Design

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
