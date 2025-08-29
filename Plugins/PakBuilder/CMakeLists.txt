# setup plugin
lyra_plugin(pakbuilder)

# plugin sources
target_sources(lyra-pakbuilder PRIVATE
    PakBuilderTypes.h
    PakBuilderPlugin.cpp
)
