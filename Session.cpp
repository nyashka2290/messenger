#include "Session.h"
#include <iostream>

Session::Session(tcp::socket socket) : socket_(std::move(socket)) {}

void Session::start() {
    initialization_read_and_continue();
}

void Session::initialization_read_and_continue() {
    auto self(shared_from_this());
    socket_.async_read_some(
            boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    data_[length] = '\0';
                    std::string name(data_, length);
                    std::cout << "his info:\n name: " << name << std::endl;
                    user.update_name(name);
                    do_write(length);
                } else if (ec == boost::asio::error::eof || ec == boost::asio::error::connection_reset) {
                    std::cout << "Client disconnected gracefully" << std::endl;
                } else {
                    std::cerr << "Read error: " << ec.message() << std::endl;
                }
            });
}

void Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
            boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    data_[length] = '\0';
                    std::cout << "Message received:\n from: " << user.get_name() + "\n what: " << data_ << std::endl;
                    do_write(length);
                } else if (ec == boost::asio::error::eof || ec == boost::asio::error::connection_reset) {
                    std::cout << "Client disconnected gracefully" << std::endl;
                } else {
                    std::cerr << "Read error: " << ec.message() << std::endl;
                }
            });
}

void Session::do_write(std::size_t length) {
    auto self(shared_from_this());
    std::string reply = "Message received:" + std::string(data_, length);
    boost::asio::async_write(
            socket_, boost::asio::buffer(reply),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    do_read(); // Продолжаем читать
                } else {
                    std::cerr << "Write error: " << ec.message() << std::endl;
                }
            });
}
