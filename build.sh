#!/bin/bash

DEFAULT_COMPILER="gcc-13"
# Check if a compiler is passed as an argument, otherwise use the default
if [ -z "$1" ]; then
    C_COMPILER="$DEFAULT_COMPILER"
else
    C_COMPILER="$1"
fi

echo "Using compiler: $C_COMPILER"

# If build does not exists, create it.
mkdir -p build
cd build/

# Comment out to get debug build, ASAN etc.
# cmake -D CMAKE_BUILD_TYPE=Debug -S ..
cmake  -DCMAKE_BUILD_TYPE=Warn -DCMAKE_C_STANDARD=23 -DCMAKE_C_COMPILER="$C_COMPILER" ..
cmake --build . -- -j$(nproc)
exit $?
