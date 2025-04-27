#include "Session.h"
#include <utility>
#include <sstream>
#include <functional>

std::unordered_map<std::string, std::shared_ptr<User>>* Session::users = nullptr;

void Session::set_users(std::unordered_map<std::string, std::shared_ptr<User>>& server_users) {
    users = &server_users;
}

Session::Session(tcp::socket socket) : socket_(std::move(socket)) {}

void Session::initialize(const std::function<void(User)>& callback) {
    auto self(shared_from_this());
    socket_.async_read_some(
            boost::asio::buffer(data_, max_length),
            [this, self, callback](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    data_[length] = '\0';
                    std::string name(data_, length);
                    if (name[name.size() - 1] == '\n') {
                        name.pop_back();
                    }
                    std::cout << "his info:\n name: " << name << std::endl;
                    User temp_user;
                    temp_user.update_name(name);
                    callback(temp_user);
                    do_write("Welcome, " + name + "\n");
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
                    std::string message(data_, length);
                    if (user_) {
                        std::cout << "Message received:\n from: " << user_->get_name() << "\n what: " << message << std::endl;
                    } else {
                        std::cout << "Message received from unknown user: " << message << std::endl;
                    }
                    if (message[message.size() - 1] == '\n') {
                        message.pop_back();
                    }
                    std::istringstream iss(message);
                    std::string command;
                    std::cout << "Raw message: '" << message << "'" << std::endl;
                    iss >> command;
                    //std::cout << "command: '" << command << "'" << std::endl;
                    if (command == "/msg") {
                        std::string target_nick, target_message;
                        iss >> target_nick;
                        std::getline(iss, target_message);
                        if (!target_nick.empty() && !target_message.empty()) {
                            auto target_user = users->find(target_nick);
                            if (target_user != users->end()) {
                                for (const auto& weak_session : target_user->second->get_sessions()) {
                                    if (auto session = weak_session.lock()) {
                                        session->do_write(user_->get_name() + " says: " + target_message + "\n");
                                    }
                                }
                                do_write("Message sent to " + target_nick + "\n");
                            } else {
                                do_write("User not found: " + target_nick + "\n");
                            }
                        } else {
                            do_write("Invalid /msg command. Usage: /msg <nick> <message>\n");
                        }
                    } else {
                        do_write("Message received: " + message + "\n");
                    }
                } else if (ec == boost::asio::error::eof || ec == boost::asio::error::connection_reset) {
                    std::cout << "Client disconnected gracefully" << std::endl;
                } else {
                    std::cerr << "Read error: " << ec.message() << std::endl;
                }
                do_read();
            });
}

void Session::do_write(const std::string& message) {
    auto self(shared_from_this());
    boost::asio::async_write(
            socket_, boost::asio::buffer(message),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (ec) {
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