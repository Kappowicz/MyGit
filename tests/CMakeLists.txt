cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        main
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

FetchContent_MakeAvailable(googletest)

add_executable(unit_tests test_main.cpp)

target_link_libraries(unit_tests PRIVATE gtest_main MyGitLib)

include(GoogleTest)
gtest_discover_tests(unit_tests)
