#ifndef WEBBER_HPP
#define WEBBER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <unordered_map>
#include <fstream>
#include <functional>

namespace webber {
    enum HTTPMethod {
        GET,    // requests a representation of the specified resource
        POST,   // submits an entity to the specified resource
        PUT,    // replaces all current representations of the target resource with the request content
        DELETE, // deletes the specified resource
        PATCH,  // applies partial modifications to a resource
        OPTIONS,// describes the communication options for the target resource
        HEAD,   // asks for a response identical to a GET request, but without a response body
        TRACE,  // performs a message loop-back test along the path to the target resource
        CONNECT,// establishes a tunnel to the server identified by the target resource
        UNKNOWN
    };

    class Request {
        public:
            Request(std::string);
            ~Request();

            std::string body(void);
        private:
            HTTPMethod method;
            std::string path;
            std::string version;
            std::unordered_map<std::string, std::string> headers;
            std::string _body;  // same name with member method body()

        friend class Webber;
    };

    class Response {
        public:
            Response(int);
            ~Response();
            void send(const std::string data);
            void render(const std::string filename);
            void render(std::ifstream& file);
        private:
            int client_fd;
    };

    using router_func_t = std::function<void(Request&, Response&)>;

    class Middleware {
    public:
        std::function<void(Request&, Response&)> func;
        Middleware* next = nullptr;
        void operator()(void);
    private:
        Request& req;
        Response& res;
    };

    class Webber {
        public:
            Webber();
            ~Webber();
            void get(const std::string path, router_func_t callback);
            void post(const std::string path, router_func_t callback);
            void listen(int port);
        
        private:
            int sock;
            struct ::sockaddr_in server;
            std::unordered_map<std::string,
                std::unordered_map<HTTPMethod, router_func_t>> routes;
            Middleware* middlewares = nullptr;

            void start_server(int port);
            void start_client(int client);
    };
}

#endif
