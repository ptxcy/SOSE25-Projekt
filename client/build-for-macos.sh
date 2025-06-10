# cd core
# mkdir -p include
# cd ..
mkdir -p lib
cd lib
mkdir -p build_mac
cd build_mac
cmake -DCMAKE_BUILD_TYPE=Debug ../..
cmake --build .
cd ..
cd ..
./build_mac
