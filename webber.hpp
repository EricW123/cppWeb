#ifndef WEBBER_HPP
#define WEBBER_HPP

#include <sys/socket.h>
#include <netinet/in.h>

namespace webber {
    class Request {
        public:
            Request();
            ~Request();
    };

    class Response {
        public:
            Response();
            ~Response();
            void send(const char* data);
    };

    class Webber {
        public:
            Webber();
            ~Webber();
            void get(const char* path, void (*callback)(Request*, Response*));
            void post(const char* path, void (*callback)(Request*, Response*));
            void listen(int port);
        
        private:
            int sock;
            struct ::sockaddr_in server;

            void start_server(int port);
            void start_client(int client);
    };
}

#endif