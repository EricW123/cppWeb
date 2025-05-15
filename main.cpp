#include <iostream>

#include "webber.hpp"


int main(int argc, char* argv[]) {
    webber::Webber app = webber::Webber();

    app.use([](webber::Request& req, webber::Response& res, webber::vvfunc_t next) {
        std::cout << "Middleware 1" << std::endl;
        next();
    });

    // app.get("/", [](webber::Request& req, webber::Response& res) {
    //     res.send("<h2>Hello, World!</h2>");
    // });
    app.get("/hello", [](webber::Request& req, webber::Response& res) {
        std::cout << "Hello, World!" << std::endl;
        res.render("./public/hello.html");
    });
    // app.get("/styles.css", [](webber::Request& req, webber::Response& res) {
    //     res.render("./public/styles.css");
    // });
    // app.get("/script.js", [](webber::Request& req, webber::Response& res) {
    //     res.render("./public/script.js");
    // });
    app.post("/log", [](webber::Request& req, webber::Response& res) {
        std::cout << "=====================" << std::endl;
        std::cout << req.body() << std::endl;
        std::cout << "=====================" << std::endl;
    });

    app.listen(3000);

    return 0;
}
