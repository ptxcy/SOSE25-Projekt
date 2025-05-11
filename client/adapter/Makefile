# HTTP Adapter Makefile

# Compiler settings
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

# Include and library paths
INCLUDES = -I/usr/local/include -I/opt/homebrew/include
LDFLAGS = -L/usr/local/lib -L/opt/homebrew/lib
LIBS = -lcpr -lcurl

# Target binaries
TARGET = http_adapter_test
TARGET_LIB = libhttp_adapter.a

# Source files
SRC = http-adapter.cpp
TEST_SRC = test_http_adapter.cpp

# Object files
OBJ = $(SRC:.cpp=.o)
TEST_OBJ = $(TEST_SRC:.cpp=.o)

# Default target
all: $(TARGET_LIB)

# Create static library
$(TARGET_LIB): $(OBJ)
	ar rcs $@ $^

# Build test executable
$(TARGET): $(TARGET_LIB) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(TEST_OBJ) $(LDFLAGS) -L. -lhttp_adapter $(LIBS)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Dependencies
http-adapter.o: http-adapter.cpp http-adapter.h
test_http_adapter.o: test_http_adapter.cpp http-adapter.h

# Clean up
clean:
	rm -f $(OBJ) $(TEST_OBJ) $(TARGET) $(TARGET_LIB)

# Install dependencies (macOS specific)
install-deps:
	brew install cpr libcurl
	# For cppcodec, you might need to clone and install it manually
	# or use another package manager if available
	# Example:
	# git clone https://github.com/tplgy/cppcodec.git
	# cd cppcodec && mkdir build && cd build
	# cmake .. && make -j4 && sudo make install

# Test rule
test: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean install-deps test
