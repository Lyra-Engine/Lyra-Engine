find_package(cmrc REQUIRED)
cmrc_add_resource_library(triangle-resources shader.slang NAMESPACE resources)

lyra_sample(triangle)
target_sources(lyra-triangle PRIVATE main.cpp)
target_link_libraries(lyra-triangle PRIVATE lyra::common)
target_link_libraries(lyra-triangle PRIVATE triangle-resources)
