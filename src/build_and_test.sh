#!/usr/bin/env bash

# Ustaw zmienne środowiskowe raz jeszcze dla pewności
export CXX=/opt/homebrew/opt/llvm/bin/clang++
export CC=/opt/homebrew/opt/llvm/bin/clang
export LDFLAGS="-L/opt/homebrew/opt/llvm/lib/c++ -Wl,-rpath,/opt/homebrew/opt/llvm/lib/c++"


cd ..
mkdir -p build
cd build || exit #in case cd fails
cmake ..
cmake --build .
ctest
