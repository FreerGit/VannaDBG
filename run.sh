#!/bin/bash
# CBA w/ CMake.
cp ./build/bin/main .
chmod +x "main"
./main "$@"
status=$?
rm "main"
exit $status
