# setup plugin
lyra_plugin(physfs)

# plugin sources
target_sources(lyra-physfs PRIVATE
    PhysFSUtils.h
    PhysFSUtils.cpp
    PhysFSPlugin.cpp
)

# packages
find_package(PhysFS REQUIRED)
target_link_libraries(lyra-physfs PRIVATE PhysFS::PhysFS)
