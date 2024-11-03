#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <thread>

#include "webber.hpp"

using namespace webber;

Request::Request() {
    // Constructor
}

Request::~Request() {
    // Destructor
}


Response::Response() {
    // Constructor
}

Response::~Response() {
    // Destructor
}

void Response::send(const char* data) {
    // Send method
}


Webber::Webber() {
    // Constructor
    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sock == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }

    int opt = 1;
    if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "setsockopt failed" << std::endl;
        return;
    }

}

Webber::~Webber() {
    // Destructor
}

void Webber::get(const char* path, void (*callback)(Request*, Response*)) {
    // Get method
}

void Webber::post(const char* path, void (*callback)(Request*, Response*)) {
    // Post method
}

void Webber::start_client(int client) {
    // Handle client method
    while (true) {
        char buffer[1024] = {0};
        std::cout << client << ": Waiting for data..." << std::endl;
        read(client, buffer, 1024);
        std::cout << client << ": ========================================" << std::endl;
        std::cout << client << ": Received: " << buffer << std::endl;
        std::cout << client << ": ========================================" << std::endl;

        std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: keep-alive\n\n<html><h3>Please stop!</h3></html>";
        send(client, response.c_str(), response.size(), 0);

        if (strstr(buffer, "Connection: keep-alive") == NULL)
            break;
    }
    close(client);
}

void Webber::start_server(int port) {
    // Handle server method
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    this->server = server;

    if (bind(this->sock, (struct sockaddr*)&server, sizeof(server)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        return;
    }
    std::cout << "Socket bound" << std::endl;

    if (::listen(this->sock, port) == -1) {
        std::cerr << "Error listening on socket" << std::endl;
        return;
    }
    std::cout << "Listening on port 3000" << std::endl;

    while (true) {
        int addr_len = sizeof(this->server);
        int client = accept(this->sock, nullptr, nullptr);

        if (client == -1) {
            std::cerr << "Error accepting connection" << std::endl;
            return;
        }
        std::cout << "Client connected: " << client << std::endl;

        std::thread client_t(&Webber::start_client, this, client);
        client_t.detach();

        sleep(0.1);
    }
}

void Webber::listen(int port) {
    // Listen method
    std::thread server_t(&Webber::start_server, this, port);
    server_t.join();

    close(this->sock);

    std::cout << "Server closed" << std::endl;
}