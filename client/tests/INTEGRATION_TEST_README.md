# Integration Test for Client WebSocket Connections

This integration test validates the complete client websocket connection flow including user creation, lobby management, and websocket connections.

## Solution Overview

This solution provides a **minimal, clean, and structured integration test** that fulfills the ticket requirements:

- **Creates two users** (with unique timestamps to avoid conflicts)
- **Creates a lobby** with the first user
- **Joins the lobby** with the second user
- **Waits for stable connection** (3 seconds)
- **Closes connections** and prints "true" if successful, "false" if failed

## Implementation

Two integration test versions are provided:

### 1. Simple Integration Test (`simple_integration_test.cpp`)

- **Minimal dependencies**: Only requires cpr library for HTTP requests
- **Clean structure**: Clear separation of concerns with proper error handling
- **Fast compilation**: No OpenGL or renderer dependencies
- **Self-contained**: Includes custom base64 encoding and HTTP adapter

### 2. Full Integration Test (`integration_test.cpp`)

- Uses full client websocket infrastructure
- Requires complete client dependencies (OpenGL, SDL2, etc.)
- More complex but closer to actual client usage

## Test Flow

The integration test performs the following operations:

1. **Create User 1**: Creates first test user (`test_user_1_<timestamp>`)
2. **Authenticate User 1**: Gets Bearer token for first user
3. **Create Lobby**: User 1 creates a new lobby (`test_lobby_<timestamp>`)
4. **Create User 2**: Creates second test user (`test_user_2_<timestamp>`)
5. **Authenticate User 2**: Gets Bearer token for second user
6. **Join Lobby**: User 2 joins the existing lobby
7. **Stable Connection**: Waits 3 seconds to ensure connections are stable
8. **Output Result**: Prints `true` if successful, `false` if failed

## Building and Running

### Quick Start (Recommended)

```bash
./run_integration_test.sh
```

### Manual Build Using CMake

```bash
mkdir -p build_cmake && cd build_cmake
cmake .. -DCMAKE_BUILD_TYPE=Release
make simple_integration_test
cd .. && ./simple_integration_test
```

## Expected Output

**When server infrastructure is running:**

```sh
Starting integration test...
Host: 127.0.0.1, Adapter Port: 8080
Lobby: test_lobby_1751049641
Creating first user: test_user_1_1751049641
User creation response: {"message":"User created successfully!"} (Status: 200)
Authenticating first user...
Authentication response: {"message":"Authenticated successfully"} (Status: 200)
Creating lobby with first user...
Lobby creation response: {"message":"Successfully created lobby!"} (Status: 200)
Creating second user: test_user_2_1751049641
User creation response: {"message":"User created successfully!"} (Status: 200)
Authenticating second user...
Authentication response: {"message":"Authenticated successfully"} (Status: 200)
Joining lobby with second user...
Lobby join response: {"message":"Successfully Joined lobby!"} (Status: 200)
Waiting for stable connection (3 seconds)...
Integration test completed successfully
true
```

**When server is not running:**

```sh
Starting integration test...
Host: 127.0.0.1, Adapter Port: 8080
Lobby: test_lobby_1751049673
Creating first user: test_user_1_1751049673
User creation response:  (Status: 0)
Failed to create first user
false
```

## Prerequisites

- Server infrastructure must be running (authproxy on port 8080)
- CMake 3.16+ or Make
- C++17 compatible compiler
- cpr library (automatically downloaded by CMake)

## Configuration

The test uses the following configuration:

- Host: `127.0.0.1`
- Adapter Port: `8080`
- WebSocket Port: `8083` (for full websocket connection)
- Unique user/lobby names with timestamps to prevent conflicts

## Code Quality Features

- **Minimal and Clean**: No bloated dependencies or unnecessary complexity
- **Structured**: Clear separation between HTTP operations and test logic
- **Good Comments**: Well-documented functions and clear variable names
- **Error Handling**: Comprehensive exception handling and status checking
- **Self-Contained**: No external configuration files required
- **Fast**: Quick compilation and execution
