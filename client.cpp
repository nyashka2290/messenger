#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "12345");
        boost::asio::connect(socket, endpoints);

        std::cout << "Connected to server!" << std::endl;

        while (true) {
            std::string message;
            std::cout << "Enter message (or 'exit' to quit): ";
            std::getline(std::cin, message);

            if (message == "exit") break;

            boost::asio::write(socket, boost::asio::buffer(message));
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}