#!/bin/bash

# Build and run integration test for client websocket connections
# This script compiles the integration test and runs it

echo "Building simple integration test..."

# Build the simple integration test using CMake
# Move up to client directory where CMakeLists.txt is located
cd ..
if [ ! -d "build_cmake" ]; then
    mkdir -p build_cmake
    cd build_cmake
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cd ..
else
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

# Return to tests directory
cd ../tests

echo "Build successful. Running integration test..."

# Run the integration test from the tests directory
# First, check if the executable has been copied here
if [ -f "./simple_integration_test" ]; then
    ./simple_integration_test
elif [ -f "../build_cmake/simple_integration_test" ]; then
    # Otherwise use the one in the build directory
    ../build_cmake/simple_integration_test
elif [ -f "../build_cmake/tests/simple_integration_test" ]; then
    # Or in build_cmake/tests if it's there
    ../build_cmake/tests/simple_integration_test
else
    cp ../build_cmake/tests/simple_integration_test ./ 2>/dev/null || \
    cp ../build_cmake/simple_integration_test ./ 2>/dev/null

    if [ -f "./simple_integration_test" ]; then
        ./simple_integration_test
    else
        echo "Error: Cannot find the integration test executable"
        exit 1
    fi
fi

# Capture the exit code
exit_code=$?

if [ $exit_code -eq 0 ]; then
    echo "Integration test completed successfully"
else
    echo "Integration test failed"
fi

exit $exit_code
