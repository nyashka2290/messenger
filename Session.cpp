#include "Session.h"

#include <utility>

Session::Session(tcp::socket socket) : socket_(std::move(socket)) {}

User Session::initialize() {
    User temp_user;
    try {
        size_t length = socket_.read_some(boost::asio::buffer(data_, max_length)); // Синхронное чтение
        data_[length] = '\0';
        std::string name(data_, length);
        std::cout << "his info:\n name: " << name << std::endl;
        temp_user.update_name(name);
        do_write(length); // Отправляем подтверждение клиенту
        return temp_user;
    } catch (const boost::system::system_error& e) {
        if (e.code() == boost::asio::error::eof || e.code() == boost::asio::error::connection_reset) {
            std::cout << "Client disconnected gracefully" << std::endl;
        } else {
            std::cerr << "Read error: " << e.what() << std::endl;
        }
        return {};
    }
}

void Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
            boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    data_[length] = '\0';
                    std::cout << "Message received:\n from: " << user_->get_name() + "\n what: " << data_ << std::endl;
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

void Session::start() {
    do_read();
}

void Session::link_user(std::shared_ptr<User> user) {
    user_ = std::move(user);
}
