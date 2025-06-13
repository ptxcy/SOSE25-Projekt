# libs
apt install libglew-dev
apt install libsdl2-dev
apt install libglm-dev
apt install libfreetype-dev

# gain excluded include files
mkdir -p core/include
git show sources:stb_image.h > core/include/stb_image.h

# cpr curl wrapper setup
cd core/include
git clone --recursive https://github.com/libcpr/cpr.git
cd cpr
cmake -B build -S . \
	  -DCPR_BUILD_TESTS=OFF \
	  -DCPR_BUILD_SHARED_LIBS=OFF \
	  -DCPR_USE_SYSTEM_CURL=OFF \
	  -DCMAKE_BUILD_TYPE=Release
cd ../..
