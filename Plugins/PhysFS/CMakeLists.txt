# setup plugin
lyra_plugin(physfs)

# plugin sources
target_sources(lyra-physfs PRIVATE PhysFSPlugin.cpp)

# packages
find_package(PhysFS)
target_link_libraries(lyra-physfs PRIVATE PhysFS::PhysFS-static)
