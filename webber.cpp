#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <sstream>
#include <fstream>

#include "webber.hpp"

using namespace webber;

Request::Request(std::string request) {
    // Constructor
    std::istringstream request_stream(request);
    std::string line;
    std::getline(request_stream, line);
    
    std::string method;

    // The first line ends with `\n`, so use getline to capture the whole line before parsing.
    // Otherwise the `\n` will be remained to later header lines and cause parsing error.
    std::istringstream line_stream(line);
    line_stream >> method >> this->path >> this->version;

    if (method.compare("GET") == 0) {
        this->method = HTTPMethod::GET;
    } else if (method.compare("POST") == 0) {
        this->method = HTTPMethod::POST;
    } else if (method.compare("PUT") == 0) {
        this->method = HTTPMethod::PUT;
    } else if (method.compare("DELETE") == 0) {
        this->method = HTTPMethod::DELETE;
    } else if (method.compare("HEAD") == 0) {
        this->method = HTTPMethod::HEAD;
    } else if (method.compare("OPTIONS") == 0) {
        this->method = HTTPMethod::OPTIONS;
    } else if (method.compare("PATCH") == 0) {
        this->method = HTTPMethod::PATCH;
    } else {
        this->method = HTTPMethod::UNKNOWN;
    }

    std::unordered_map<HTTPMethod, std::string> method_to_string = {
        {HTTPMethod::GET, "iGET"},
        {HTTPMethod::POST, "POST"},
        {HTTPMethod::PUT, "PUT"},
        {HTTPMethod::DELETE, "DELETE"},
        {HTTPMethod::HEAD, "HEAD"},
        {HTTPMethod::OPTIONS, "OPTIONS"},
        {HTTPMethod::PATCH, "PATCH"},
        {HTTPMethod::UNKNOWN, "UNKNOWN"}
    };
    std::cout << "Method: " << method_to_string[this->method] << std::endl;
    std::cout << "Path: " << this->path << std::endl;
    std::cout << "Version: " << this->version << std::endl;

    bool is_header = true;
    while (std::getline(request_stream, line)) {
        // the socket will receive a `\r\n` as the first line
        // so `if (line == '\r') break;` won't work since it will break at the first line
        if (line.empty()) break;
        if (line.back() == '\r') line.pop_back();
        if (line.empty()) { is_header = false; continue; }

        if (is_header) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                if (value.front() == ' ') value.erase(0, 1); // remove leading space
                this->headers[key] = value;
            }
        } else {
            // body
            this->_body += line;
        }
    }

    // std::cout << "-- Request Headers --" << std::endl;
    // for (auto header : this->headers) {
    //     std::cout << "\t" << header.first << ": " << header.second << std::endl;
    // }
    // std::cout << "-- End of Headers --" << std::endl;
}

std::string Request::body() {
    // Body method
    return this->_body;
}

Request::~Request() {
    // Destructor
}


Response::Response(int client_fd) {
    // Constructor
    this->client_fd = client_fd;
}

Response::~Response() {
    // Destructor
}

void Response::send(const std::string msg, std::string content_type) {
    // Wrap message with HTTP headers, send to frontend
    bool keep_alive = false;
    // TODO: keep-alive may cause bug: browser mess up with multiple files received
    // and consider received js file as css file
    // so keep-alive is disabled for now
    std::string data = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: " + content_type + "\r\n"
        "Content-Length: " + std::to_string(msg.size()) + "\r\n"
        "Connection: " + (keep_alive ? "keep-alive" : "close") + "\r\n"
        "\r\n" + msg;
    // No need to splite data into chunks if size <= ~1MB
    // size_t total_sent = 0;
    // while (total_sent < data.size()) {
    //     ssize_t sent = ::send(this->client_fd, data.data() + total_sent, data.size() - total_sent, 0);
    //     std::cout << sent << " bytes sent." << std::endl;
    //     if (sent == -1) {
    //         std::cerr << "Error sending data" << std::endl;
    //         break;
    //     }
    //     if (sent == 0) break;
    //     total_sent += sent;
    // }
    ::send(this->client_fd, data.data(), data.size(), 0);
}

void Response::render(std::ifstream& file_stream, std::string content_type) {
    std::string file;
    std::string line;

    if (!file_stream.is_open()) {
        std::cerr << "Error opening file stream" << std::endl;
        return;
    }

    while (std::getline(file_stream, line)) {
        // Read file line by line
        file += line + "\r\n";
    }

    file_stream.close();

    this->send(file, content_type);
    // sleep(1);    // for testing keep-alive
}

void Response::render(const std::string filename, std::string content_type) {
    std::ifstream file_stream(filename);
    this->render(file_stream, content_type);
}


Webber::Webber() {
    // Constructor
    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sock == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }

    int opt = 1;    // allow reuse of address and port
    if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "setsockopt failed" << std::endl;
        return;
    }
}

Webber::~Webber() {
    // Destructor
}

void Webber::get(const std::string path, router_func_t callback) {
    // Get method
    middleware_func_t mw = [path, callback](Request& req, Response& res, vvfunc_t next) {
        if (req.method == HTTPMethod::GET && req.path == path) {
            callback(req, res);
        } else {
            // If the method is not GET, call the next middleware
            if (next) next();
        }
    };
    this->middlewares.push_back(mw);
}

void Webber::post(const std::string path, router_func_t callback) {
    // Post method
    middleware_func_t mw = [path, callback](Request& req, Response& res, vvfunc_t next) {
        if (req.method == HTTPMethod::POST && req.path == path) {
            callback(req, res);
        } else {
            // If the method is not POST, call the next middleware
            if (next) next();
        }
    };
    this->middlewares.push_back(mw);
}

void Webber::nextMidd() {
    middleware_func_t mw = this->getNextMidd();
    if (mw == nullptr) return;
    mw(this->request, this->response, [this]() { this->nextMidd(); });
}

void Webber::use(middleware_func_t middleware) {
    // TODO: app.use() register middlewares to front part
    // app.get() register to second part
    // other default handlers register to last part
    // (req,res) |->app.use()->|->app.get()->|->default routers->|->404 Not Found |
    // but user should be able to use app.use() and app.get() at any order
    // currently the order of the middlewares is not guaranteed yet.
    this->middlewares.push_back(middleware);
};

middleware_func_t Webber::getNextMidd() {
    if (this->middlewares.size() == 0) return nullptr;
    if (this->current_midd >= this->middlewares.size()) return nullptr;
    return this->middlewares[this->current_midd++];
}

void Webber::start_client(int client) {
    std::string buffer(1024, 0);
    while (true) {
        // buffer.data() returns a `const void*` before C++17, but `recv()` needs a `void*` pointer
        // In C++17 or higher, can use `recv(client, buffer.data(), buffer.size(), 0);`
        ssize_t res = recv(client, &buffer[0], buffer.size(), 0);
        if (res == 0) {
            // recv() returns 0 means socket is disconnected by peer
            // Browser may send 2-3 TCP requests when accessing a page,
            // some of them are just empty and will send FIN as soon as received SYN-ACK
            // just ignore them.
            std::cout << client << ": disconnected by peer" << std::endl;
            break;
        }
        if (res < 0) {
            // Only when recv() returns -1 means error, positive means data received, and 0 means disconnected by peer
            std::cerr << client << ": reading failed" << std::endl;
            break;
        }

        this->request = Request(buffer);
        this->response = Response(client);

        middleware_func_t default_route = [](Request& req, Response& res, vvfunc_t next) {
            std::string public_path = "./public" + req.path;
            if (req.path == "/")
                public_path = "./public/index.html";
            std::cout << "Public path: " << public_path << std::endl;
            std::ifstream file(public_path);
            if (req.method == HTTPMethod::GET && file.good()) {
                if (req.path.find(".css") != std::string::npos) {
                    res.render(file, "text/css");
                } else if (req.path.find(".js") != std::string::npos) {
                    res.render(file, "text/javascript");
                } else if (req.path.find(".html") != std::string::npos) {
                    res.render(file, "text/html");
                } else {
                    res.render(file);
                }
                file.close();
            } else {
                if (next) next();
            }
        };
        this->middlewares.push_back(default_route);

        middleware_func_t error_route = [](Request& req, Response& res, vvfunc_t next) {
            res.send("<h3>404 Not Found</h3>");
        };
        this->middlewares.push_back(error_route);
        
        std::cout << "-- Midd --" << std::endl;
        this->current_midd = 0;
        this->nextMidd();
        std::cout << "-- End of Midd --" << std::endl;

        if (buffer.find("Connection: close") != std::string::npos) {
            // If the client sends a request with "Connection: close", close the connection
            std::cout << client << ": disconnected by client" << std::endl;
            break;
        }
    }

    close(client);
    std::cout << client << ": socket closed" << std::endl;
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

    if (::listen(this->sock, SOMAXCONN) == -1) {
        std::cerr << "Error listening on socket" << std::endl;
        return;
    }
    std::cout << "Listening on port 3000" << std::endl;

    while (true) {
        ssize_t client = accept(this->sock, nullptr, nullptr);

        if (client == -1) {
            std::cerr << "Error accepting connection" << std::endl;
            return;
        }
        std::cout << "New client connected: " << client << std::endl;

        std::thread client_t(&Webber::start_client, this, client);
        client_t.detach();
    }
}

void Webber::listen(int port) {
    // Listen method
    std::thread server_t(&Webber::start_server, this, port);
    server_t.join();

    close(this->sock);

    std::cout << "Server closed" << std::endl;
}
