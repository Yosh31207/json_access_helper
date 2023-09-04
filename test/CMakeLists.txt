cmake_minimum_required(VERSION 3.0)
project(json_helper_test CXX)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_Declare(
    boost
    URL https://boostorg.jfrog.io/artifactory/main/release/1.83.0/source/boost_1_83_0.tar.bz2
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest boost)
enable_testing()

add_executable(json_helper_test
    ./src/boost_json_source.cpp
    ./src/json_helper_test.cpp
)
set_target_properties(json_helper_test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(json_helper_test
    PUBLIC
    ./build/_deps/boost-src/
    ../src/
)
target_compile_features(json_helper_test
    PUBLIC
    cxx_std_17
)
target_compile_options(json_helper_test
    PUBLIC
    -Wall
    -Wextra
)
target_link_libraries(
    json_helper_test
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(json_helper_test)
