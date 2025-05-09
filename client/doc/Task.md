# Client Task: AuthProxy Communication Adapters

## Feature Request: Implement HTTP and WebSocket Adapters

**Goal:** Create C++ adapters within the client application to handle communication with the `authproxy` microservice for user authentication and lobby management, and to establish a WebSocket connection for real-time features with the `calculation_unit`.

**Components:**
1.  **HTTP Adapter:** Handles RESTful API calls to `authproxy` using the **Crow C++ library**.
2.  **WebSocket Adapter:** Handles establishing and managing a WebSocket connection to the `calculation_unit` after authentication via `authproxy`.

---

## HTTP Adapter (`http-adapter.cpp`) Tasks

Implement functions within `http-adapter.cpp` using the **Crow C++ library** ([https://crowcpp.org/](https://crowcpp.org/)) to support the following `authproxy` endpoints. **All HTTP communication (request bodies and expected response bodies) MUST use the JSON format.**

1.  **Create User:**
    *   **Method:** `POST`
    *   **Path:** `/authproxy/user`
    *   **Request Headers:**
        *   `Authorization`: Bearer `<token>` (Obtained from Authenticate User step)
    *   **Request Body (JSON):**
        ```json
        {
          "username": "example_user",
          "password": "example_password"
        }
        ```
    *   **Response (JSON):** Expected: `{"message": "Successfully created user!"}` on success (Status 200). (Need clarification from PL on error response structure/status).

2.  **Authenticate User:**
    *   **Method:** `GET`
    *   **Path:** `/authproxy/authenticate`
    *   **Request Headers:**
        *   `Authorization`: Basic `<Base64 encoded username:password>`
    *   **Response (JSON):** Expected: `{"message": "Authentication was successful!"}` (Status 200). The Bearer token is returned in the `Authenticate` response header. (Need clarification from PL if JSON body should also contain the token).

3.  **Create Lobby:**
    *   **Method:** `POST`
    *   **Path:** `/authproxy/lobbys`
    *   **Request Headers:**
        *   `Authorization`: Bearer `<token>` (Obtained from Authenticate User step)
    *   **Request Body (JSON):**
        ```json
        {
          "lobbyName": "example_lobby",
          // "password": "optional_password" // Omit field if no password is set
        }
        ```
    *   **Response (JSON):** Expected: `{"message": "Successfully created Lobby! Please start Web Socket Connection with Lobby Info!"}` on success (Status 200). (Need clarification from PL on error response structure/status).

---

## WebSocket Adapter (`websocket-adapter.cpp`) Task

Implement functionality within `websocket-adapter.cpp` (using a suitable C++ WebSocket library with MessagePack support) to:

1.  **Connect to Calculation Unit:**
    *   **Target Service:** `calculation_unit`
    *   **Path:** `/msgpack` (Based on `calculation_unit/src/main.rs`)
    *   **Headers:**
        *   `Authorization`: Bearer `<token>` (Obtained from HTTP authentication step 2)
    *   **Purpose:** Establish a persistent connection after successful authentication for real-time communication.
    *   **Data Format:** All messages sent and received over this WebSocket connection **MUST use the MessagePack (msgpack) format.**

---

## Connection Flow Summary

The intended sequence of operations for the client is:

1.  **Register:** Call `POST /authproxy/user` to create a new user account (Requires Bearer token - *Note: This seems unusual, typically registration doesn't require prior auth. Clarify with PL if this endpoint needs auth*).
2.  **Login:** Call `GET /authproxy/authenticate` using Basic Auth to receive a Bearer token (in the `Authenticate` header).
3.  **Create Lobby:** Call `POST /authproxy/lobbys` using the Bearer token to create a game lobby.
4.  **Connect WebSocket:** Establish a WebSocket connection to `calculation_unit` at `/msgpack` using the Bearer token for authorization.

---

## Notes & Questions for Project Leader (PL)

*   **Crow C++ Library Integration:** How should the Crow library be integrated into the client project? (e.g., Header-only include, package manager like Conan/vcpkg, submodule?).
*   **Data Formats:** Confirmed: HTTP uses JSON, WebSocket uses MessagePack.
*   **WebSocket Endpoint (`/calculate` vs `/msgpack`):** Confirmed: The Rust service (`calculation_unit`) exposes `/msgpack` for MessagePack WebSocket connections.
*   **WebSocket Token Validation:** How does `calculation_unit` validate the Bearer token received in the WebSocket handshake? Does it call `authproxy`? (Codebase doesn't show validation logic).
*   **Auth Header Format:** Confirmed: `Authorization: Bearer <token>` is the correct format for authenticated HTTP requests and WebSocket connection headers (based on `authproxy` middleware).
*   **WebSocket Library:** Is a specific C++ WebSocket library preferred, or should one be selected? Must support MessagePack and custom headers.
*   **Base URLs:** Confirmed: For client development outside Docker, use `http://localhost:8080` for `authproxy` and `ws://localhost:8082` for `calculation_unit`.
*   **API Responses & Errors:**
    *   What are the expected error response bodies (JSON structure) and HTTP status codes for the defined HTTP endpoints?
    *   How should WebSocket connection errors or invalid messages (in MessagePack format) be handled?
*   **Optional Lobby Password:** Confirmed: Omit the `password` field in the JSON payload for `POST /authproxy/lobbys` if no password is desired.
*   **User Creation Auth:** The `POST /authproxy/user` endpoint currently has `validateAuthorization` middleware. Does creating a user really require an existing valid Bearer token? This seems counter-intuitive for registration.




asynchroner call
endpunkte bauen
create lobby, gibt bool zurück