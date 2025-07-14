cmake_minimum_required(VERSION 3.16..3.30)
project(MeinProjekt)

set(CMAKE_CXX_STANDARD 17)

# Einfache statische Linking-Konfiguration
set(BUILD_SHARED_LIBS OFF)

# FetchContent-Modul einbinden
include(FetchContent)

# Bibliotheken aus Online-Repositories laden
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG origin/master
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG master
)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.11.0
)

FetchContent_Declare(
    boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.88.0

)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.28.5
)

FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-13-3
)

set(CMAKE_POLICY_DEFAULT_CMP0000 NEW)

# Make all libraries available
FetchContent_MakeAvailable(cpr fmt spdlog stb SDL2 freetype)

# GLEW über find_package einbinden
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

# stb_image.h automatisch herunterladen
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/core/include")
file(DOWNLOAD
    "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/include/stb_image.h"
)

include_directories(${CMAKE_BINARY_DIR})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/core/include")

# Verzeichnisse für Quellcode und Header-Dateien
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${boost_SOURCE_DIR}
    ${stb_SOURCE_DIR}
)

# Define necessary preprocessor directives for WebSocket++
add_definitions(
    -DASIO_STANDALONE
    -DGLM_ENABLE_EXPERIMENTAL
)

# Bibliotheken zum Linken - verwende CMake-Targets wo möglich
set(LIBS "-framework OpenGL" GLEW::GLEW SDL2::SDL2-static pthread freetype cpr::cpr fmt::fmt spdlog::spdlog)

# Debug und Release Flags
set(CMAKE_CXX_FLAGS_DEBUG "-pg -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fno-gcse")

# Quell- und Header-Dateien finden
file(GLOB_RECURSE SRCS_CORE "core/*.cpp")
file(GLOB_RECURSE SRCS_SCRIPT "script/*.cpp")
set(SRCS ${SRCS_CORE} ${SRCS_SCRIPT} main.cpp)

# Integration test files - only include websocket-related core files
set(INTEGRATION_TEST_SRCS
    core/base.cpp
    core/websocket.cpp
    tests/integration_test.cpp
)

# Simple integration test - minimal dependencies
set(SIMPLE_INTEGRATION_TEST_SRCS
    tests/simple_integration_test.cpp
)

# Erstelle die ausführbare Datei
add_executable(build_mac ${SRCS})

# Create integration test executable
add_executable(integration_test ${INTEGRATION_TEST_SRCS})

# Create simple integration test executable
add_executable(simple_integration_test ${SIMPLE_INTEGRATION_TEST_SRCS})

# Verknüpfe die Bibliotheken ohne -static Flag auf macOS
target_link_libraries(build_mac ${LIBS})
target_link_libraries(integration_test cpr::cpr)
target_link_libraries(simple_integration_test cpr::cpr)

# Set output directory to the Client folder instead of build
set_target_properties(build_mac
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(integration_test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

set_target_properties(simple_integration_test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
)
