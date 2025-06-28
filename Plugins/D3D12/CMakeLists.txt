# setup plugin
lyra_plugin(d3d12)

find_package(D3D12)

# plugin sources
target_sources(lyra-d3d12 PRIVATE
    Plugin.cpp
    D3D12Utils.cpp
    D3D12Heap.cpp
    D3D12Factory.cpp
    D3D12Adapter.cpp
    D3D12Device.cpp
    D3D12Swapchain.cpp
    D3D12Frame.cpp
    D3D12Fence.cpp
    D3D12Buffer.cpp
    D3D12Texture.cpp
    D3D12Sampler.cpp
    D3D12Shader.cpp
    D3D12Pipeline.cpp
    D3D12Layout.cpp
)

# link with D3D12
target_link_libraries(lyra-d3d12 PRIVATE ${D3D12_LIBRARIES})
target_include_directories(lyra-d3d12 PRIVATE ${DXGI_INCLUDE_DIR})
target_include_directories(lyra-d3d12 PRIVATE ${D3D12_INCLUDE_DIR})
