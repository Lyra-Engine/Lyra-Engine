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
find_package(Vulkan REQUIRED) # temporary solution, need to find a way to compile dxc
target_link_libraries(lyra-dxc PRIVATE dxc_inc Vulkan::dxc_lib spirvreflect)
