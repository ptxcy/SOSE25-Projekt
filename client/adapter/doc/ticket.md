# Feature Request: Implementation of HTTP and WebSocket Adapters for AuthProxy

## Description

We require two adapters to facilitate communication with AuthProxy: an HTTP adapter and a WebSocket adapter. These adapters will enable user authentication and connection to lobbies.

## Tasks

### HTTP Adapter

Develop an HTTP adapter to support the following requests:

1. **Create User**  
    **POST** `http://localhost:8080/user`  
    **Payload:**  
    ```json
    {
         "username": "string",
         "password": "string"
    }
    ```

2. **Authenticate User**  
    **GET** `http://localhost:8080/authenticate`  
    **Headers:**  
    - `Authorization: Basic Base64(username:password)`

3. **Create Lobby**  
    **POST** `http://localhost:8080/lobbys`  
    **Payload:**  
    ```json
    {
         "lobbyName": "string",
         "password": "optional string"
    }
    ```  
    **Headers:**  
    - `Authorization: Bearer <token>`

### WebSocket Adapter

Develop a WebSocket adapter to enable connections to the `/calculate` endpoint using the `Authorization` header.

## Connection Workflow

1. **Create a User:**  
    The user creates a new account by sending a `POST` request to `http://localhost:8080/user`.

2. **Authenticate:**  
    The user authenticates by sending a `GET` request to `http://localhost:8080/authenticate` and receives a Bearer token.

3. **Create a Lobby:**  
    The user creates a lobby by sending a `POST` request to `http://localhost:8080/lobbys` with the `Authorization` header.

4. **Connect via WebSocket:**  
    The user connects to the `/calculate` endpoint using the WebSocket adapter and includes the `Authorization` header.
