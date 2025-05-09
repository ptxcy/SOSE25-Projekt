#!/bin/bash

# Navigate to the script's directory (client/)
cd "$(dirname "$0")"

# Prüfen, ob die Bibliotheken bereits installiert sind, falls nicht, installiere sie
if [ ! -d "./external/include/crow" ] || [ ! -f "./external/include/asio.hpp" ]; then
    echo "Erforderliche Bibliotheken nicht gefunden. Werden installiert..."
    bash ./install_dependencies.sh
fi

echo "Compiling client application and generating compile_commands.json..."

# Include-Pfad festlegen
INCLUDE_FLAGS="-I$(pwd)/external/include"

# OpenSSL wird von Crow für HTTPS-Verbindungen benötigt (optional, für MacOS)
OPENSSL_FLAGS=""
if [ "$(uname)" == "Darwin" ]; then
    if [ -d "/usr/local/opt/openssl" ]; then
        OPENSSL_FLAGS="-I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib"
    elif [ -d "/opt/homebrew/opt/openssl" ]; then
        OPENSSL_FLAGS="-I/opt/homebrew/opt/openssl/include -L/opt/homebrew/opt/openssl/lib" 
    fi
fi

# Alle notwendigen Compiler-Flags
COMPILER_FLAGS="-std=c++17 -pthread ${INCLUDE_FLAGS} ${OPENSSL_FLAGS}"
LINKER_FLAGS="-lssl -lcrypto"

echo "Using compiler flags: ${COMPILER_FLAGS}"
echo "Using linker flags: ${LINKER_FLAGS}"

# Wrap the compilation command with bear to generate compile_commands.json
# Compile main.cpp with the appropriate include paths
bear -- clang++ main.cpp -o client_app ${COMPILER_FLAGS} ${LINKER_FLAGS}

# Check if compilation/linking was successful
if [ $? -eq 0 ]; then
    echo -e "\033[32mCompilation successful.\033[0m" # Green text
    echo -e "\033[34mRunning client_app...\033[0m" # Blue text
    ./client_app
else
    echo -e "\033[31mCompilation/Linking failed.\033[0m" # Red text
    exit 1
fi
