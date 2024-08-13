#!/bin/bash

clean() {
    echo "Cleaning the build environment..."
    rm -rf ./build
}

build() {
    DEFAULT_COMPILER="gcc-13"
    # Check if a compiler is passed as an argument, otherwise use the default
    if [ -z "$1" ]; then
        C_COMPILER="$DEFAULT_COMPILER"
    else
        C_COMPILER="$1"
    fi
    echo "Using compiler: $C_COMPILER"

    mkdir -p build
    cd build/
    cmake -DCMAKE_BUILD_TYPE=Warn -DCMAKE_C_STANDARD=23 -DCMAKE_C_COMPILER="$C_COMPILER" ..
    cmake --build . -- -j$(nproc)
    cd ..
}

test() {
    echo "Running tests..."
    cd build/
    make run_tests
    local status=$?
    cd ..
    return $status
}

run() {
    echo "Running the application..."
    cp ./build/bin/VannaDBG .
    chmod +x "VannaDBG"
    ./VannaDBG "$@"
    local status=$?
    rm "VannaDBG"
    return $status
}

ci() {
    ./bin/act
    docker system prune -a --volumes -f
}

if [ "$1" == "build" ]; then
    build "$2"
    exit $?
elif [ "$1" == "rebuild" ]; then
    clean
    build "$2"
    exit $?
elif [ "$1" == "test" ]; then
    test
    exit $?
elif [ "$1" == "run" ]; then
    shift
    run "$@"
    exit $?
elif [ "$1" == "ci" ]; then
    ci
else
    echo "Usage: $0 {build|rebuild|test|ci|run [args]}"
    exit 1
fi

exit 0