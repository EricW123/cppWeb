#include <iostream>

#include "webber.hpp"


int main(int argc, char* argv[]) {
    webber::Webber app = webber::Webber();

    // app.get("/", [](webber::Request* req, webber::Response* res) {
    //     res->send("Hello, World!");
    // });
    app.listen(3000);

    return 0;
}