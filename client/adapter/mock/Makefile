# Makefile for mock HTTP adapter

# Compiler settings
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Targets
TARGETS = mock_test example

# Build all programs
all: $(TARGETS)

# Create mock library
libmock_http_adapter.a: mock_http_adapter.o
	ar rcs $@ $^

# Compile mock implementation
mock_http_adapter.o: mock_http_adapter.cpp mock_http_adapter.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test program
mock_test: test_mock.cpp mock_http_adapter.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile example program
example: example.cpp mock_http_adapter.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

# Run test
test: mock_test
	./mock_test

# Run example
run-example: example
	./example

# Clean up
clean:
	rm -f $(TARGETS) *.o *.a

.PHONY: all clean test run-example
