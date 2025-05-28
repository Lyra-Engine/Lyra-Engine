# setup plugin
lyra_plugin(dxc)

# plugin sources
target_sources(lyra-dxc PRIVATE
    Plugin.cpp
    DxcUtils.cpp
    DxcCompiler.cpp
)

# enable microsoft extension
target_compile_options(lyra-dxc PRIVATE -fms-extensions)

# link dependencies
find_package(dxc REQUIRED)
find_package(spirvreflect REQUIRED)
find_package(Vulkan REQUIRED COMPONENTS dxc) # temporary solution, need to find a way to compile dxc
target_link_libraries(lyra-dxc PRIVATE Vulkan::dxc_lib dxc_inc spirvreflect)
