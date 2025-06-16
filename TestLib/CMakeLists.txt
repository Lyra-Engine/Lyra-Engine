lyra_sample(testlib)
find_package(stb REQUIRED)
find_package(doctest REQUIRED)
target_sources(lyra-testlib PRIVATE main.cpp helper.cpp)
target_link_libraries(lyra-testlib PRIVATE stb doctest::doctest)
target_include_directories(lyra-testlib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# test cases
add_subdirectory(test.00)
