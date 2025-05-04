# HTTP Adapter (`http-adapter.cpp`) using Crow

## Overview

This C++ file provides functions for sending HTTP GET and POST requests to the AuthProxy service using the **Crow C++** library.

**Important Considerations:**

*   **Crow for Client:** This implementation leverages the Crow C++ framework for client operations.
*   **Dependencies:** This implementation requires the **Crow C++ Library** to be correctly installed and accessible (headers and linking).
*   **URL Parsing:** The code includes a regex-based URL parsing implementation.
*   **Base64:** For Basic Authentication, Crow's built-in base64 encoding utilities are used.

## Key Components

### 1. `HttpResponse` Struct

```cpp
struct HttpResponse {
    int http_code;
    std::string body;
    std::string authorizationHeader; // Stores 'Authenticate' or 'Authorization' header value
};
```

*   Returns information about the HTTP response.
*   `http_code`: HTTP status code.
*   `body`: Response body string.
*   `authorizationHeader`: Value of the `Authenticate` (preferred) or `Authorization` header from the response.

### 2. Base64 Encoding

```cpp
std::string base64_encode(const std::string& input)
```

* Utilizes Crow's built-in `crow::utility::base64encode` for Base64 encoding.
* Used for creating Basic Authentication headers.

### 3. URL Parsing

```cpp
struct UrlComponents {
    std::string protocol;
    std::string host;
    std::string port;
    std::string path;
};

UrlComponents parse_url(const std::string& url)
```

*   Parses URLs using regex to extract protocol, host, port, and path.
*   Handles default ports (80 for HTTP, 443 for HTTPS) if not specified.

### 4. `sendGetRequest` Function

```cpp
HttpResponse sendGetRequest(const std::string& url, const std::string& authorizationHeader = "");
```

*   **Purpose**: Sends an HTTP GET request.
*   **Implementation**: Uses Crow to:
    *   Parse the URL.
    *   Create a Crow application and request object.
    *   Set up appropriate headers (Host, User-Agent, Authorization if provided).
    *   Send the request and collect the response.
    *   Extract status code, body, and relevant headers.
*   **Parameters**:
    *   `url`: Target URL.
    *   `authorizationHeader` (optional): Full header string (e.g., "Basic ..." or "Bearer ...").
*   **Returns**: An `HttpResponse` struct with status code, response body, and authorization header.
*   **Error Handling**: Uses try-catch blocks for error handling, logging to stderr.

### 5. `sendPostRequest` Function

```cpp
HttpResponse sendPostRequest(const std::string& url, 
                           const std::string& postData, 
                           const std::string& contentType = "application/json", 
                           const std::string& authorizationHeader = "");
```

*   **Purpose**: Sends an HTTP POST request.
*   **Implementation**: Uses Crow similar to `sendGetRequest`, but:
    *   Sets the HTTP method to POST.
    *   Includes the request body and Content-Type header.
*   **Parameters**:
    *   `url`: Target URL.
    *   `postData`: Request body (JSON string for AuthProxy).
    *   `contentType`: Content-Type header value.
    *   `authorizationHeader` (optional): Full authorization header string.
*   **Returns**: An `HttpResponse` struct.
*   **Error Handling**: Uses try-catch blocks for error handling, logging to stderr.

### 6. AuthProxy Helper Functions

The adapter provides convenience methods for specific AuthProxy endpoints:

#### 6.1 `createUser` Function

```cpp
HttpResponse createUser(const std::string& baseUrl, const std::string& username, const std::string& password, const std::string& bearerToken);
```

*   **Purpose**: Creates a user in AuthProxy.
*   **Endpoint**: `POST /authproxy/user`
*   **Parameters**:
    *   `baseUrl`: Base URL for AuthProxy (e.g., "http://localhost:8080").
    *   `username`: Username to create.
    *   `password`: Password for the new user.
    *   `bearerToken`: Authentication token for the request (Bearer token).
*   **Returns**: HTTP response from AuthProxy.

#### 6.2 `authenticateUser` Function

```cpp
HttpResponse authenticateUser(const std::string& baseUrl, const std::string& username, const std::string& password);
```

*   **Purpose**: Authenticates a user against AuthProxy.
*   **Endpoint**: `GET /authproxy/authenticate`
*   **Parameters**:
    *   `baseUrl`: Base URL for AuthProxy.
    *   `username`: Username for authentication.
    *   `password`: Password for authentication.
*   **Returns**: HTTP response containing Authenticate header with Bearer token upon successful authentication.

#### 6.3 `createLobby` Function

```cpp
HttpResponse createLobby(const std::string& baseUrl, const std::string& lobbyName, const std::string& bearerToken, const std::string& password = "");
```

*   **Purpose**: Creates a new lobby in AuthProxy.
*   **Endpoint**: `POST /authproxy/lobbys`
*   **Parameters**:
    *   `baseUrl`: Base URL for AuthProxy.
    *   `lobbyName`: Name of the lobby to create.
    *   `bearerToken`: Authentication token for the request (Bearer token).
    *   `password`: Optional password for the lobby.
*   **Returns**: HTTP response from AuthProxy.

## Dependencies

*   **Crow C++ Library**: Used for HTTP client functionality.
*   **Standard C++ Libraries**: string, iostream, stdexcept, vector, map, regex.

## Usage Example

```cpp
// Example: Authenticate user, then create a lobby
std::string baseUrl = "http://localhost:8080";

// Authenticate
auto authResponse = authenticateUser(baseUrl, "myusername", "mypassword");
if (authResponse.http_code == 200) {
    std::string bearerToken = authResponse.authorizationHeader;
    // Bearer token is in the authorizationHeader field
    
    // Create a lobby using the token
    auto lobbyResponse = createLobby(baseUrl, "My Game Lobby", bearerToken);
    if (lobbyResponse.http_code == 200) {
        // Lobby created successfully
        std::cout << "Lobby created: " << lobbyResponse.body << std::endl;
    }
}
```
