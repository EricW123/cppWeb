#include <iostream>

#include "webber.hpp"


int main(int argc, char* argv[]) {
    webber::Webber app = webber::Webber();

    app.get("/", [](webber::Request* req, webber::Response* res) {
        res->send("<h2>Hello, World!</h2>");
    });
    app.get("/hello", [](webber::Request* req, webber::Response* res) {
        res->send("<h2>Hello2, World!</h2>");
    });
    app.post("/log", [](webber::Request* req, webber::Response* res) {
        std::cout << req->body() << std::endl;
    });

    app.listen(3000);

    return 0;
}
