#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <iostream>
#include "User.h"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket socket);
    void start();
    void initialize(const std::function<void(User)>& callback);
    void link_user(std::shared_ptr<User> user);

    static std::unordered_map<std::string, std::shared_ptr<User>>* users;
    static void set_users(std::unordered_map<std::string, std::shared_ptr<User>>& server_users);
private:
    void do_read();
    void do_write(const std::string& message);

    static const size_t max_length = 1024;
    char data_[max_length];
    tcp::socket socket_;
    std::shared_ptr<User> user_;
};