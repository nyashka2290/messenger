#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include "additional_settings.h"

#include "User.h"

using boost::asio::ip::tcp;

void ask_name(User& user) {
    std::cout << "write your name" << std::endl;
    std::string new_name;
    std::getline(std::cin, new_name);
    user.update_name(new_name);
}

void communication(User& user) {
    try {
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "12345");
        boost::asio::connect(socket, endpoints);

        std::cout << "Connected to server!" << std::endl;
        boost::asio::write(socket, boost::asio::buffer(user.get_name()));

        while (true) {
            std::string message;
            std::cout << "Enter message (or 'exit' to quit):" << std::endl;
            std::getline(std::cin, message);

            if (message == "exit") break;

            boost::asio::write(socket, boost::asio::buffer(message));

        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    set_echo(false);
    User user;
    ask_name(user);
    communication(user);
    return 0;
}