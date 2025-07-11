lyra_sample(testkit)
find_package(stb REQUIRED)
find_package(doctest REQUIRED)
target_sources(lyra-testkit PRIVATE
    main.cpp
    ./common/helper.cpp
    ./common/uniform.cpp
    ./common/geometry.cpp
    ./common/render.cpp
    ./common/texture.cpp
    ./common/app.cpp
)
target_link_libraries(lyra-testkit PRIVATE stb doctest::doctest)
target_include_directories(lyra-testkit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common)

# test cases
add_subdirectory(depth_test)
add_subdirectory(stencil_test)
add_subdirectory(texture_sampling)
add_subdirectory(graphics_pipeline)

# add custom target to run test kit
find_package(Python3 REQUIRED COMPONENTS Interpreter)
message(STATUS "Python interpreter: ${Python3_EXECUTABLE}")
message(STATUS "Python version: ${Python3_VERSION}")
add_custom_target(
    testkit
    COMMAND ${Python3_EXECUTABLE} -u ${PROJECT_SOURCE_DIR}/Scripts/runtest.py $<TARGET_FILE:lyra-testkit> --directory ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Run TestKit regression with HTML report!"
    DEPENDS lyra-testkit
    VERBATIM)
