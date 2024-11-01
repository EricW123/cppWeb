#ifndef WEBBER_HPP
#define WEBBER_HPP

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
            void listen(int port);
    };
}

#endif