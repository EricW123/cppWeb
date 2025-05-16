#ifndef WEBBER_HPP
#define WEBBER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <unordered_map>
#include <fstream>
#include <functional>
#include <vector>
#include <string>


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
            void send(const std::string data, std::string content_type = "text/html");
            void render(const std::string filename, std::string content_type = "text/html");
            void render(std::ifstream& file, std::string content_type = "text/html");
            
            void set(const std::string name, std::string value);
            std::string get(const std::string name);
        private:
            int client_fd;
            std::unordered_map<std::string, std::string> attributes;
    };

    using vvfunc_t = std::function<void()>;
    using router_func_t = std::function<void(Request&, Response&)>;
    using middleware_func_t = std::function<void(Request&, Response&, vvfunc_t)>;

    class Webber {
        public:
            Webber();
            ~Webber();
            void use(middleware_func_t func);
            void get(const std::string path, router_func_t callback);
            void post(const std::string path, router_func_t callback);
            void listen(int port);
            void run();

        private:
            int sock;
            int port;
            struct ::sockaddr_in server;
            Request request = Request("");
            Response response = Response(0);
            std::vector<middleware_func_t> routers;
            std::vector<middleware_func_t> middlewares;
            size_t current_midd = 0;

            void start_server(int port);
            void start_client(int client);
            void default_midds(void);
            void nextMidd(void);
            void runMidd();
            middleware_func_t getNextMidd(void);
    };
}

#endif
