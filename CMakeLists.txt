cmake_minimum_required(VERSION 3.31.6)
project(MyGit)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(mygit
        include/mygit.h
        src/main.cpp
        src/mygit.cpp)
