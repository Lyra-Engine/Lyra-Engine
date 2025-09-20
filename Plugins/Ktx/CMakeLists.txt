# setup plugin
lyra_plugin(ktx)

# plugin sources
target_sources(lyra-ktx PRIVATE
    KtxPlugin.cpp
)

find_package(ktx REQUIRED)
target_link_libraries(lyra-ktx PRIVATE KTX::ktx)
