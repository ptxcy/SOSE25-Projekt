#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include <crow.h>
#include <regex>
#include <asio.hpp>

// Structure to encapsulate HTTP response data
struct HttpResponse {
    int http_code;
    std::string body;
    std::string authorizationHeader; // Stores 'Authenticate' or 'Authorization' header value
};

// Base64 encoding function (required for Basic Auth)
std::string base64_encode(const std::string& input) {
    // Using Crow's base64encode function which requires the data and size
    return crow::utility::base64encode(input, input.size());
}

std::string getDefaultPort(const std::string& protocol) {
    return protocol == "https" ? "443" : "80";
}

// Simple URL parsing function
struct UrlComponents {
    std::string protocol;
    std::string host;
    std::string port;
    std::string path;
};

UrlComponents parse_url(const std::string& url) {
    UrlComponents result;

    auto protocol_end = url.find("://");
    if (protocol_end == std::string::npos) {
        throw std::runtime_error("Invalid URL format: " + url);
    }
    result.protocol = url.substr(0, protocol_end);

    auto host_start = protocol_end + 3;
    auto path_start = url.find('/', host_start);
    auto port_start = url.find(':', host_start);

    if (path_start == std::string::npos) {
        result.path = "/";
    } else {
        result.path = url.substr(path_start);
    }

    if (port_start != std::string::npos && (path_start == std::string::npos || port_start < path_start)) {
        result.host = url.substr(host_start, port_start - host_start);
        result.port = url.substr(port_start + 1, path_start - port_start - 1);
    } else {
        result.host = url.substr(host_start, path_start - host_start);
        result.port = getDefaultPort(result.protocol);
    }

    return result;
}

// Function to send an HTTP GET request using ASIO directly
HttpResponse sendGetRequest(const std::string& url, const std::string& authorizationHeader = "") {
    HttpResponse response;
    response.http_code = 500; // Default error code
    
    try {
        // Parse URL
        auto urlParts = parse_url(url);
        
        // Create ASIO io_context
        asio::io_context io_context;
        
        // Resolve the server hostname
        asio::ip::tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(urlParts.host, urlParts.port);
        
        // Create and connect the socket
        asio::ip::tcp::socket socket(io_context);
        asio::connect(socket, endpoints);
        
        // Prepare the HTTP request
        std::string request = "GET " + urlParts.path + " HTTP/1.1\r\n";
        request += "Host: " + urlParts.host + ":" + urlParts.port + "\r\n";
        request += "User-Agent: CrowClient/1.0\r\n";
        request += "Accept: */*\r\n";
        
        // Add authorization header if provided
        if (!authorizationHeader.empty()) {
            request += "Authorization: " + authorizationHeader + "\r\n";
        }
        
        // Add empty line to indicate end of headers
        request += "Connection: close\r\n\r\n";
        
        // Send the request
        asio::write(socket, asio::buffer(request));
        
        // Read the response
        asio::streambuf response_buffer;
        asio::read_until(socket, response_buffer, "\r\n");
        
        // Check if response is valid HTTP
        std::istream response_stream(&response_buffer);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        
        if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
            throw std::runtime_error("Invalid response");
        }
        
        response.http_code = status_code;
        
        // Read the response headers
        asio::read_until(socket, response_buffer, "\r\n\r\n");
        
        // Process the headers
        std::string header;
        while (std::getline(response_stream, header) && header != "\r") {
            if (header.find("Authenticate:") == 0 || header.find("authenticate:") == 0) {
                response.authorizationHeader = header.substr(13); // Remove "Authenticate: "
                // Remove carriage return if present
                if (!response.authorizationHeader.empty() && response.authorizationHeader.back() == '\r') {
                    response.authorizationHeader.pop_back();
                }
            } else if (header.find("Authorization:") == 0 || header.find("authorization:") == 0) {
                response.authorizationHeader = header.substr(14); // Remove "Authorization: "
                // Remove carriage return if present
                if (!response.authorizationHeader.empty() && response.authorizationHeader.back() == '\r') {
                    response.authorizationHeader.pop_back();
                }
            }
        }
        
        // Read the response body
        std::ostringstream response_body;
        if (response_buffer.size() > 0) {
            response_body << &response_buffer;
        }
        
        // Continue reading remaining data
        asio::error_code error;
        while (asio::read(socket, response_buffer, asio::transfer_at_least(1), error)) {
            response_body << &response_buffer;
        }
        
        if (error != asio::error::eof) {
            throw asio::system_error(error);
        }
        
        response.body = response_body.str();
        
        // Log errors for debug purposes
        if (response.http_code >= 400) {
            std::cerr << "HTTP Error: " << response.http_code << std::endl;
            std::cerr << "Response Body: " << response.body << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during GET request: " << e.what() << std::endl;
        response.body = std::string("Error: ") + e.what();
    }
    
    return response;
}

// Function to send an HTTP POST request using ASIO directly
HttpResponse sendPostRequest(const std::string& url, 
                           const std::string& postData, 
                           const std::string& contentType = "application/json", 
                           const std::string& authorizationHeader = "") {
    
    HttpResponse response;
    response.http_code = 500; // Default error code
    
    try {
        // Parse URL
        auto urlParts = parse_url(url);
        
        // Create ASIO io_context
        asio::io_context io_context;
        
        // Resolve the server hostname
        asio::ip::tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(urlParts.host, urlParts.port);
        
        // Create and connect the socket
        asio::ip::tcp::socket socket(io_context);
        asio::connect(socket, endpoints);
        
        // Prepare the HTTP request
        std::string request = "POST " + urlParts.path + " HTTP/1.1\r\n";
        request += "Host: " + urlParts.host + ":" + urlParts.port + "\r\n";
        request += "User-Agent: CrowClient/1.0\r\n";
        request += "Accept: */*\r\n";
        request += "Content-Type: " + contentType + "\r\n";
        request += "Content-Length: " + std::to_string(postData.length()) + "\r\n";
        
        // Add authorization header if provided
        if (!authorizationHeader.empty()) {
            request += "Authorization: " + authorizationHeader + "\r\n";
        }
        
        // Add empty line to indicate end of headers and then the post data
        request += "Connection: close\r\n\r\n" + postData;
        
        // Send the request
        asio::write(socket, asio::buffer(request));
        
        // Read the response
        asio::streambuf response_buffer;
        asio::read_until(socket, response_buffer, "\r\n");
        
        // Check if response is valid HTTP
        std::istream response_stream(&response_buffer);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        
        if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
            throw std::runtime_error("Invalid response");
        }
        
        response.http_code = status_code;
        
        // Read the response headers
        asio::read_until(socket, response_buffer, "\r\n\r\n");
        
        // Process the headers
        std::string header;
        while (std::getline(response_stream, header) && header != "\r") {
            if (header.find("Authenticate:") == 0 || header.find("authenticate:") == 0) {
                response.authorizationHeader = header.substr(13); // Remove "Authenticate: "
                // Remove carriage return if present
                if (!response.authorizationHeader.empty() && response.authorizationHeader.back() == '\r') {
                    response.authorizationHeader.pop_back();
                }
            } else if (header.find("Authorization:") == 0 || header.find("authorization:") == 0) {
                response.authorizationHeader = header.substr(14); // Remove "Authorization: "
                // Remove carriage return if present
                if (!response.authorizationHeader.empty() && response.authorizationHeader.back() == '\r') {
                    response.authorizationHeader.pop_back();
                }
            }
        }
        
        // Read the response body
        std::ostringstream response_body;
        if (response_buffer.size() > 0) {
            response_body << &response_buffer;
        }
        
        // Continue reading remaining data
        asio::error_code error;
        while (asio::read(socket, response_buffer, asio::transfer_at_least(1), error)) {
            response_body << &response_buffer;
        }
        
        if (error != asio::error::eof) {
            throw asio::system_error(error);
        }
        
        response.body = response_body.str();
        
        // Log errors for debug purposes
        if (response.http_code >= 400) {
            std::cerr << "HTTP Error: " << response.http_code << std::endl;
            std::cerr << "Response Body: " << response.body << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during POST request: " << e.what() << std::endl;
        response.body = std::string("Error: ") + e.what();
    }
    
    return response;
}

// Helper function to create user with AuthProxy
HttpResponse createUser(const std::string& baseUrl, const std::string& username, const std::string& password, const std::string& bearerToken) {
    std::string url = baseUrl + "/authproxy/user";
    
    // Create JSON payload
    std::string jsonPayload = R"({"username":")" + username + R"(","password":")" + password + R"("})";
    
    // Create authorization header with Bearer token
    std::string authHeader = "Bearer " + bearerToken;
    
    // Send POST request
    return sendPostRequest(url, jsonPayload, "application/json", authHeader);
}

// Helper function to authenticate user with AuthProxy
HttpResponse authenticateUser(const std::string& baseUrl, const std::string& username, const std::string& password) {
    std::string url = baseUrl + "/authproxy/authenticate";
    
    // Create Basic Auth header
    std::string credentials = username + ":" + password;
    std::string encodedCredentials = base64_encode(credentials);
    std::string authHeader = "Basic " + encodedCredentials;
    
    // Send GET request with Basic Auth header
    return sendGetRequest(url, authHeader);
}

// Helper function to create lobby with AuthProxy
HttpResponse createLobby(const std::string& baseUrl, const std::string& lobbyName, const std::string& bearerToken, const std::string& password = "") {
    std::string url = baseUrl + "/authproxy/lobbys";
    
    // Create JSON payload - include password only if provided
    std::string jsonPayload;
    if (password.empty()) {
        jsonPayload = R"({"lobbyName":")" + lobbyName + R"("})";
    } else {
        jsonPayload = R"({"lobbyName":")" + lobbyName + R"(","password":")" + password + R"("})";
    }
    
    // Create authorization header with Bearer token
    std::string authHeader = "Bearer " + bearerToken;
    
    // Send POST request
    return sendPostRequest(url, jsonPayload, "application/json", authHeader);
}


// createUser
// createLobby
// authenticateUser
// username:password base 64 encode (value) danach string basic concatinate
// header:
// GET http://authproxy:8080/authenticate,
// "Authorization":"Basic Base64(username:password)"