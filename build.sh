#!/bin/bash

# If build does not exists, create it.
mkdir -p build

cd build/
# Comment out to get debug build, ASAN etc.
# cmake -D CMAKE_BUILD_TYPE=Debug -S ..
cmake -DCMAKE_BUILD_TYPE=Warn -DCMAKE_C_COMPILER=gcc ..
cmake --build . -- -j$(nproc)
exit $?
