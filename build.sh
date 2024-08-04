#!/bin/bash
cd build/
# Comment out to get debug build, ASAN etc.
cmake -D CMAKE_BUILD_TYPE=Debug -S ..
# cmake ..
make
exit $?
