#include "websocket-adapter.h"
#include "message-pack-helper.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <msgpack.hpp>

// Structure for a simple message
struct Message {
    std::string type;
    std::string content;
    
    MSGPACK_DEFINE(type, content); // Makes this struct serializable with MessagePack
};

// Thread-safe message queue
class MessageQueue {
public:
    void push(const Message& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(msg);
        cv_.notify_one();
    }
    
    bool pop(Message& msg) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // Wait for a message or stop signal
        cv_.wait(lock, [this] { return !queue_.empty() || stop_; });
        
        if (stop_ && queue_.empty()) {
            return false;
        }
        
        msg = queue_.front();
        queue_.pop();
        return true;
    }
    
    void stop() {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
        cv_.notify_all();
    }
    
private:
    std::queue<Message> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_ = false;
};

// Process messages in a separate thread
void processMessages(MessageQueue& queue, std::atomic_bool& running) {
    Message msg;
    while (running && queue.pop(msg)) {
        std::cout << "Processing message of type: " << msg.type << std::endl;
        std::cout << "Content: " << msg.content << std::endl;
        
        // Simulate processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "Message processing thread shutting down" << std::endl;
}

int main(int argc, char* argv[]) {
    // Message queue and control flags
    MessageQueue message_queue;
    std::atomic_bool running(true);
    
    // Start message processing thread
    std::thread processing_thread(processMessages, std::ref(message_queue), std::ref(running));
    
    // Authenticate and get a token first (this would come from the HTTP adapter in practice)
    std::optional<std::string> token = authenticateOnServer("username", "password");
    if (!token) {
        std::cerr << "Failed to authenticate" << std::endl;
        running = false;
        message_queue.stop();
        if (processing_thread.joinable()) {
            processing_thread.join();
        }
        return 1;
    }
    
    // Message handling callback
    auto messageCallback = [&message_queue](const std::string& data, bool is_binary) {
        if (!is_binary) {
            std::cout << "Received text message: " << data << std::endl;
            
            // Create a simple message for text messages
            Message msg;
            msg.type = "text";
            msg.content = data;
            message_queue.push(msg);
        } else {
            std::cout << "Received binary message" << std::endl;
            
            try {
                // Deserialize the MessagePack data
                auto msg = msgpack_helper::unpack_from_message<Message>(data, true);
                message_queue.push(msg);
            }
            catch (const std::exception& e) {
                std::cerr << "Error deserializing message: " << e.what() << std::endl;
            }
        }
    };
    
    // Connection status callback
    auto connectionCallback = [&running](bool connected) {
        if (connected) {
            std::cout << "Connected to AuthProxy WebSocket server" << std::endl;
        } else {
            std::cout << "Disconnected from AuthProxy WebSocket server" << std::endl;
            running = false;
        }
    };
    
    // Connect to the authproxy service using the dedicated function
    auto client = connectToAuthProxyWebSocket(token.value(), "localhost", "8080", "/api/ws", messageCallback, connectionCallback);
    
    if (!client) {
        std::cerr << "Failed to connect to WebSocket server" << std::endl;
        running = false;
        message_queue.stop();
        if (processing_thread.joinable()) {
            processing_thread.join();
        }
        return 1;
    }
    
    // Send a test message
    Message test_msg;
    test_msg.type = "hello";
    test_msg.content = "Hello, AuthProxy WebSocket Server!";
    
    auto packed_data = msgpack_helper::pack(test_msg);
    if (!sendMessagePack(client, packed_data)) {
        std::cerr << "Failed to send MessagePack data" << std::endl;
    }
    
    // Keep the application running until the user presses Enter
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    
    // Clean up
    running = false;
    message_queue.stop();
    client->close();
    
    if (processing_thread.joinable()) {
        processing_thread.join();
    }
    
    return 0;
}
