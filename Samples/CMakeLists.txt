lyra_sample(test)
find_package(glm REQUIRED)
target_sources(lyra-test PRIVATE test.cpp)
target_link_libraries(lyra-test PRIVATE lyra::common glm::glm)
