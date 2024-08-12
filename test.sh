#!/bin/bash
# CBA w/ CMake.
cd build/
make run_tests
status=$?
exit $status
