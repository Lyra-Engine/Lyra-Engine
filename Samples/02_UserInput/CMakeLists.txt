cmrc_add_resource_library(user-input-resources shader.slang NAMESPACE resources)

lyra_sample(user-input)
target_sources(lyra-user-input PRIVATE main.cpp)
target_link_libraries(lyra-user-input PRIVATE user-input-resources)
