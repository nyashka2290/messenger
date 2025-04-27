#pragma once

#include <boost/asio.hpp>
#include <memory>
#include "Session.h"
#include <iostream>

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_context& io_context, short port);
    void add_session_with_user(std::shared_ptr<Session>& session, User& user);
private:
    void do_accept();

    tcp::acceptor acceptor_;

    std::unordered_map<std::string, std::shared_ptr<User>> users;
};