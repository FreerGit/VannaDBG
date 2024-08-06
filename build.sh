#!/bin/bash

# If build does not exists, create it.
mkdir -p build

cd build/
# Comment out to get debug build, ASAN etc.
# cmake -D CMAKE_BUILD_TYPE=Debug -S ..
cmake -D CMAKE_BUILD_TYPE=Warn ..
make
exit $?
