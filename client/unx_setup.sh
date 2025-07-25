# check for input
if [ -z "$1" ]; then
	echo "please specify package manager"
	exit 1
fi

# libs
$1 install cmake
$1 install libcurl4-openssl-dev
$1 install libglew-dev
$1 install libsdl2-dev
$1 install libglm-dev
$1 install libfreetype-dev
$1 install libmsgpack-dev

# gain excluded include files
mkdir -p core/include
curl -o core/include/stb_image.h \
	 https://raw.githubusercontent.com/ptxcy/SOSE25-Projekt/refs/heads/sources/stb_image.h

# cpr curl wrapper setup
cd core/include
git clone --recursive https://github.com/libcpr/cpr.git
cd cpr
cmake -B build -S . \
	  -DCPR_BUILD_TESTS=OFF \
	  -DCPR_USE_SYSTEM_CURL=ON \
	  -DBUILD_SHARED_LIBS=OFF \
	  -DCMAKE_BUILD_TYPE=Release
cd build
make
make install
cd ../..
rm -rf cpr
cd ../..
