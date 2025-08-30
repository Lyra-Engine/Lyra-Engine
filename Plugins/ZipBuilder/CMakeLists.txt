# setup plugin
lyra_plugin(zipbuilder)

# plugin sources
target_sources(lyra-zipbuilder PRIVATE
    ZipBuilderTypes.h
    ZipBuilderPlugin.cpp
)

find_package(miniz REQUIRED)
target_link_libraries(lyra-zipbuilder PRIVATE miniz)
