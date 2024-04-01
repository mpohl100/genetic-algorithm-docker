#!/bin/bash

# Set default build directory
BUILD_DIR=$(pwd)/../build

# Check if a build directory is provided as a command line argument
if [ "$#" -gt 0 ]; then
    BUILD_DIR="$1"
fi

# Create the build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR"

sudo apt-get update && sudo apt-get install -y -o Acquire::Retries=3 lcov

rm -rf Debug
conan build .. --options coverage=True -b="missing" -pr:b="$(pwd)/../conan_profiles/gcc11_cpp20_debug.prof" -pr:h="$(pwd)/../conan_profiles/gcc11_cpp20_debug.prof" -s build_type=Debug -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True 
./Debug/tests/tests

cd Debug
make coverage