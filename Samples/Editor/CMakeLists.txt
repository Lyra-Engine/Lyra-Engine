lyra_sample(editor)
find_package(stb REQUIRED)
target_sources(lyra-editor PRIVATE
    main.cpp
    application.h
    application.cpp
)
target_link_libraries(lyra-editor PRIVATE stb)
