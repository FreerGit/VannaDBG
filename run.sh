#!/bin/bash
# CBA w/ CMake.
cp ./build/bin/VannaDBG .
chmod +x "VannaDBG"
./VannaDBG "$@"
status=$?
rm "VannaDBG"
exit $status
