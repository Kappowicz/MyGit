cmake_minimum_required(VERSION 4.0)
project(MyGit)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(MyGit
        include/mygit.h
        src/main.cpp
        src/mygit.cpp)
