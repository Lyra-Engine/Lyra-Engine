# setup plugin
lyra_plugin(slang)

# plugin sources
target_sources(lyra-slang PRIVATE
    Plugin.cpp
    SlangUtils.cpp
)

# link dependencies
find_package(slang REQUIRED)
target_link_libraries(lyra-slang PRIVATE slang::slang)
