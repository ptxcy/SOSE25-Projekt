# Testing the HTTP and WebSocket Adapters

This document describes how to test the HTTP and WebSocket adapters in the SOSE25-Projekt.

## Prerequisites

Before running the tests, ensure the following prerequisites are met:

1. **Backend services are running:**
   ```bash
   cd /Users/pni/Uni/SOSE25-Projekt
   ./start-backend.sh
   ```

2. **Required libraries are installed:**
   - cpprest (Microsoft C++ REST SDK)
   - cpr (C++ Requests)
   - cppcodec (for Base64 encoding)

   If you haven't installed these libraries, you can install them using:
   ```bash
   # For macOS with Homebrew
   brew install cpprestsdk cpp-httplib cppcodec
   
   # Or use the project's installer script
   cd /Users/pni/Uni/SOSE25-Projekt/client
   ./install_cpprest_lib.sh
   ```

## Building and Running the Tests

To build and run the adapter tests:

1. **Navigate to the test directory:**
   ```bash
   cd /Users/pni/Uni/SOSE25-Projekt/client/adapter/test
   ```

2. **Build the test program:**
   ```bash
   ./build_test.sh
   ```

3. **Run the test program:**
   ```bash
   ./adapter_test
   ```

## What the Tests Do

The test program performs the following operations:

### HTTP Adapter Tests

1. **Create User:**
   - Attempts to create a test user via POST to `/user`
   - Reports success or failure

2. **Authenticate User:**
   - Attempts to authenticate the user via GET to `/authenticate` with Basic Auth
   - Obtains a JWT token if successful

3. **Create Lobby:**
   - Attempts to create a test lobby via POST to `/lobbys` with JWT token
   - Reports success or failure

4. **Complete Auth Process:**
   - Tests the convenience function that combines all the above steps

### WebSocket Adapter Tests

1. **Connect to /calculate endpoint:**
   - Establishes a WebSocket connection to the `/calculate` endpoint
   - Sends a text message
   - Waits for any response
   - Closes the connection

2. **Connect to Calculation Unit:**
   - Establishes a WebSocket connection to the calculation unit's `/msgpack` endpoint
   - Sends a binary message (simulating MessagePack data)
   - Waits for any response
   - Closes the connection

## Troubleshooting

If you encounter issues:

1. **Backend Services:**
   - Ensure the backend services are running with `docker ps`
   - Verify their ports with `docker ps`

2. **Compilation Errors:**
   - Check that all required libraries and their development files are installed
   - Adjust the include and library paths in `build_test.sh` if needed

3. **Connection Issues:**
   - Check the host and port values in the test file
   - Ensure no firewall is blocking the connections

4. **Authentication Issues:**
   - Ensure the MongoDB database is running and accessible
   - Try recreating the user with a different username if the previous one exists
