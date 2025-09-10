# setup plugin
lyra_plugin(pak)

# plugin sources
target_sources(lyra-pak PRIVATE
    PakTypes.h
    PakPlugin.cpp
)
