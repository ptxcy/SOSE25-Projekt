cmake_minimum_required(VERSION 3.10)
project(MeinProjekt)

set(CMAKE_CXX_STANDARD 17)

# Define platform-specific settings
if(APPLE)
    # macOS specific settings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/opt/homebrew/include -I/opt/homebrew/include/freetype2")
    link_directories(/opt/homebrew/lib)
elseif(UNIX AND NOT APPLE)
    # Linux specific settings
    # No special settings needed for default paths
endif()

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
    glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG origin/master
)

FetchContent_Declare(
    boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.82.0
)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)

# Bibliotheken verfügbar machen
FetchContent_MakeAvailable(cpr fmt spdlog glew stb)

# Verzeichnisse für Quellcode und Header-Dateien
include_directories(
    /usr/include/freetype2
    /usr/include/libpng16
    /opt/homebrew/include/freetype2
    ${PROJECT_SOURCE_DIR}/include
    ${glew_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/../external/websocketpp
    ${boost_SOURCE_DIR}
    ${stb_SOURCE_DIR}
)

# Define necessary preprocessor directives for WebSocket++
add_definitions(
    -DASIO_STANDALONE
    -DGLM_ENABLE_EXPERIMENTAL
)

# Bibliotheken zum Linken
if(APPLE)
    set(LIBS "-framework OpenGL" GLEW SDL2 pthread freetype cpr curl ssl crypto z fmt spdlog)
else()
    set(LIBS GL GLEW SDL2 pthread freetype cpr curl ssl crypto z fmt spdlog)
endif()

# Debug und Release Flags
set(CMAKE_CXX_FLAGS_DEBUG "-pg -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fno-gcse")

# Quell- und Header-Dateien finden
file(GLOB_RECURSE SRCS_CORE "core/*.cpp")
file(GLOB_RECURSE SRCS_SCRIPT "script/*.cpp")
set(SRCS ${SRCS_CORE} ${SRCS_SCRIPT} main.cpp)

# Erstelle die ausführbare Datei
add_executable(Client ${SRCS})

# Verknüpfe die Bibliotheken
target_link_libraries(Client ${LIBS})

# Set output directory to the client folder instead of build
set_target_properties(Client
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
