cmake_minimum_required(VERSION 3.25)
project(MyGit)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

add_library(MyGitLib STATIC
        src/mygit.cpp
)

target_include_directories(MyGitLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(mygit src/main.cpp)
target_link_libraries(mygit PRIVATE MyGitLib)

enable_testing()
add_subdirectory(tests)
