# HTTP and WebSocket Adapter Makefile

# Compiler settings
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

# Include and library paths
INCLUDES = -I/usr/local/include -I/opt/homebrew/include
LDFLAGS = -L/usr/local/lib -L/opt/homebrew/lib
LIBS = -lcpr -lcurl -lcpprest -lssl -lcrypto -lboost_system -lpthread -lmsgpack

# Target binaries
TARGET = http_adapter_test
TARGET_LIB = libhttp_adapter.a
WS_TARGET = websocket_example
WS_TARGET_LIB = libwebsocket_adapter.a

# Source files
HTTP_SRC = http-adapter.cpp
WS_SRC = websocket-adapter.cpp
TEST_SRC = test_http_adapter.cpp
WS_TEST_SRC = websocket_example.cpp

# Object files
HTTP_OBJ = $(HTTP_SRC:.cpp=.o)
WS_OBJ = $(WS_SRC:.cpp=.o)
TEST_OBJ = $(TEST_SRC:.cpp=.o)
WS_TEST_OBJ = $(WS_TEST_SRC:.cpp=.o)

# Default target
all: $(TARGET_LIB) $(WS_TARGET_LIB) $(WS_TARGET)

# Create static libraries
$(TARGET_LIB): $(HTTP_OBJ)
	ar rcs $@ $^

$(WS_TARGET_LIB): $(WS_OBJ)
	ar rcs $@ $^

# Build test executables
$(TARGET): $(TARGET_LIB) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(TEST_OBJ) $(LDFLAGS) -L. -lhttp_adapter $(LIBS)

$(WS_TARGET): $(WS_TARGET_LIB) $(HTTP_OBJ) $(WS_TEST_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(WS_TEST_OBJ) $(HTTP_OBJ) $(LDFLAGS) -L. -lwebsocket_adapter $(LIBS)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Dependencies
http-adapter.o: http-adapter.cpp http-adapter.h
websocket-adapter.o: websocket-adapter.cpp websocket-adapter.h message-pack-helper.h
test_http_adapter.o: test_http_adapter.cpp http-adapter.h
websocket_example.o: websocket_example.cpp websocket-adapter.h message-pack-helper.h http-adapter.h

# Clean up
clean:
	rm -f $(HTTP_OBJ) $(WS_OBJ) $(TEST_OBJ) $(WS_TEST_OBJ) $(TARGET) $(TARGET_LIB) $(WS_TARGET) $(WS_TARGET_LIB)

# Install dependencies (macOS specific)
install-deps:
	brew install cpr libcurl cpprestsdk msgpack-cxx boost
	# For cppcodec, you might need to clone and install it manually
	# or use another package manager if available

# WebSocket specific targets
websocket: $(WS_TARGET)

run-websocket-example: $(WS_TARGET)
	./$(WS_TARGET)
	# Example:
	# git clone https://github.com/tplgy/cppcodec.git
	# cd cppcodec && mkdir build && cd build
	# cmake .. && make -j4 && sudo make install

# Test rule
test: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean install-deps test
