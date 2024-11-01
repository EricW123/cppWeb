#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

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
}

Webber::~Webber() {
    // Destructor
}

void Webber::get(const char* path, void (*callback)(Request*, Response*)) {
    // Get method
}

void Webber::listen(int port) {
    // Listen method
    
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;  // will point to the results
    struct addrinfo *p;  // temporary pointer

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
    char ipstr[INET6_ADDRSTRLEN];


    if ((status = getaddrinfo("www.google.com", NULL, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("  %s: %s\n", ipver, ipstr);
    }
}