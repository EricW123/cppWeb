#include <iostream>
#include <ctime>

#include "webber.hpp"


using Req = webber::Request;
using Res = webber::Response;
using Next = webber::vvfunc_t;
using MW = webber::middleware_func_t;

int main(int argc, char* argv[]) {
    webber::Webber app = webber::Webber();

    // app.get("/", [](webber::Request& req, webber::Response& res) {
    //     res.send("<h2>Hello, World!</h2>");
    // });
    app.get("/hello", [](webber::Request& req, webber::Response& res) {
        std::cout << "Hello, World!" << std::endl;
        res.render("./public/hello.html");
    });
    app.post("/log", [](webber::Request& req, webber::Response& res) {
        std::cout << "=====================" << std::endl;
        std::cout << req.body() << std::endl;
        std::cout << "=====================" << std::endl;
    });


    MW logger = [&](Req& req, Res& res, Next next) {
        next();
        std::cout << "Time used: " << res.get("X-Response-Time") << "ms" << std::endl;
    };
    app.use(logger);

    MW timer = [&](Req& req, Res& res, Next next) {
        std::clock_t start = std::clock();
        next();
        std::clock_t end = std::clock();
        double elapsed_time = double(end - start) / CLOCKS_PER_SEC * 1000;
        res.set("X-Response-Time", std::to_string(elapsed_time));
    };
    app.use(timer);

    app.listen(3000);
    app.run();

    return 0;
}
