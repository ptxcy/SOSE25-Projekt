#!/bin/bash

# Dieses Skript lädt die Crow C++ Header-only Bibliothek, ASIO und Boost.Beast herunter und installiert sie im Projektverzeichnis

# Erstelle einen Ordner für externe Bibliotheken, falls noch nicht vorhanden
mkdir -p external/include

echo "Downloade ASIO C++ Bibliothek..."
curl -L -o asio.zip https://github.com/chriskohlhoff/asio/archive/asio-1-20-0.zip

echo "Entpacke ASIO..."
unzip -q asio.zip

echo "Kopiere ASIO Header-Dateien..."
cp -r asio-asio-1-20-0/asio/include/* external/include/

echo "Downloade Boost.Beast für WebSocket-Support..."
curl -L -o beast.tar.gz https://github.com/boostorg/beast/archive/develop.tar.gz

echo "Entpacke Boost.Beast..."
tar -xf beast.tar.gz

echo "Erstelle Boost-Verzeichnisstruktur..."
mkdir -p external/include/boost

echo "Kopiere Boost.Beast Header-Dateien..."
cp -r beast-develop/include/boost/* external/include/boost/

# Boost.Beast benötigt einige weitere Boost-Header
echo "Downloade zusätzliche Boost-Header..."
curl -L -o boost_headers.tar.gz https://github.com/boostorg/boost/archive/master.tar.gz
tar -xf boost_headers.tar.gz

# Wichtige Boost-Header kopieren, die Beast benötigt
echo "Kopiere wichtige Boost-Header..."
cp -r boost-master/boost/config external/include/boost/
cp -r boost-master/boost/core external/include/boost/
cp -r boost-master/boost/assert external/include/boost/
cp -r boost-master/boost/throw_exception external/include/boost/
cp -r boost-master/boost/system external/include/boost/
cp -r boost-master/boost/mp11 external/include/boost/
cp -r boost-master/boost/logic external/include/boost/
cp -r boost-master/boost/optional external/include/boost/
cp -r boost-master/boost/endian external/include/boost/
cp -r boost-master/boost/utility external/include/boost/
cp -r boost-master/boost/type_traits external/include/boost/
cp -r boost-master/boost/preprocessor external/include/boost/
cp -r boost-master/boost/static_assert external/include/boost/
cp -r boost-master/boost/smart_ptr external/include/boost/

echo "Downloade Crow C++ Bibliothek..."
curl -L -o crow.zip https://github.com/CrowCpp/Crow/archive/master.zip

echo "Entpacke Crow..."
unzip -q crow.zip

echo "Kopiere Crow Header-Dateien..."
mkdir -p external/include/crow
cp -r Crow-master/include/* external/include/

echo "Bereinige heruntergeladene Dateien..."
rm -rf crow.zip Crow-master asio.zip asio-asio-1-20-0 beast.tar.gz beast-develop boost_headers.tar.gz boost-master

echo "Crow, ASIO und Boost.Beast wurden erfolgreich installiert in 'external/include'."
echo "Die Header-Dateien können nun mit '#include <crow.h>', '#include <asio.hpp>' oder '#include <boost/beast.hpp>' inkludiert werden."
echo "Vergiss nicht, '-I$(pwd)/external/include' zu deinem Build-Kommando hinzuzufügen."