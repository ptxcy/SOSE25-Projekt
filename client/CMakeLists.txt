cmake_minimum_required(VERSION 3.16...3.30)
project(MeinProjekt)

set(CMAKE_CXX_STANDARD 17)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_OSX_ARCHITECTURES "arm64")

# Debug/Release Flags
set(CMAKE_CXX_FLAGS_DEBUG "-pg -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fno-gcse")

add_definitions(-DASIO_STANDALONE)

# CPR dynamisch linken
find_library(CPR_LIBRARY NAMES cpr PATHS /opt/homebrew/lib REQUIRED)
include_directories(/opt/homebrew/include)

# Source files
file(GLOB_RECURSE SRCS_CORE "core/*.cpp")
file(GLOB_RECURSE SRCS_SCRIPT "script/*.cpp")
set(SRCS ${SRCS_CORE} ${SRCS_SCRIPT} main.cpp)

# Main executable only
add_executable(build_mac ${SRCS})
target_compile_definitions(build_mac PRIVATE PROJECT_PONG)
target_include_directories(build_mac PRIVATE
        /opt/homebrew/include
        /opt/homebrew/include/SDL2
        /opt/homebrew/include/freetype2
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/core/include
        ${PROJECT_SOURCE_DIR}/include
)
target_link_directories(build_mac PRIVATE /opt/homebrew/lib)
target_link_libraries(build_mac PRIVATE
        "-framework OpenGL"
        SDL2
        GLEW
        pthread
        freetype
        msgpackc
        ${CPR_LIBRARY}
)
set_target_properties(build_mac PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
