#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include "Server.h"
#include "additional_settings.h"

int main() {
    set_echo(false);
    try {
        boost::asio::io_context io_context;
        Server server(io_context, 12345);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
