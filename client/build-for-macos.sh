#!/bin/bash

echo "Installing dependencies via Homebrew..."
brew install sdl2
brew install glew
brew install glm
brew install msgpack-cxx
brew install freetype
brew install cpr

echo "Creating folders..."
mkdir -p core/include
mkdir -p lib/build_mac

echo "Downloading stb_image.h..."
curl -o core/include/stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h

echo "Running CMake build..."
cd lib/build_mac
cmake -DCMAKE_BUILD_TYPE=Debug ../..
cmake --build .

echo "Build finished!"
