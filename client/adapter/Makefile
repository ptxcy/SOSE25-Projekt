# HTTP Adapter Test Makefile

CC = g++
CXXFLAGS = -std=c++17 -Wall
TARGET = http_adapter_test

# macOS Homebrew pfade für Apple Silicon (M1/M2/M3) oder Intel
INCLUDE_DIRS = -I/usr/local/include -I/opt/homebrew/include
LIB_DIRS = -L/usr/local/lib -L/opt/homebrew/lib

# Zusätzlich Include-Verzeichnis für cppcodec, falls nötig
# Falls cppcodec in einem anderen Verzeichnis liegt, passe den Pfad an
INCLUDE_DIRS += -I$(HOME)/Downloads/cppcodec/cppcodec-0.2

# Libraries needed
LIBS = -lcpr

all: $(TARGET)

$(TARGET): http_adapter_test.cpp http-adapter.cpp http-adapter.h
	$(CC) $(CXXFLAGS) $(INCLUDE_DIRS) $(LIB_DIRS) -o $@ http_adapter_test.cpp http-adapter.cpp $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
