#!/usr/bin/env bash

cd ..
mkdir -p build
cd build || exit #in case cd fails
cmake ..
cmake --build .
ctest
