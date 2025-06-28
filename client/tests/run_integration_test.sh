#!/bin/bash

# Build and run integration test for client websocket connections
# This script compiles the integration test and runs it

echo "Building simple integration test..."

# Build the simple integration test using CMake
if [ ! -d "build_cmake" ]; then
    mkdir -p build_cmake
    cd build_cmake
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cd ..
fi

cd build_cmake
make simple_integration_test

# Check if build was successful
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

cd ..

echo "Build successful. Running integration test..."

# Run the integration test from the tests directory
./simple_integration_test

# Capture the exit code
exit_code=$?

if [ $exit_code -eq 0 ]; then
    echo "Integration test completed successfully"
else
    echo "Integration test failed"
fi

exit $exit_code
