#pragma once

#include <boost/asio.hpp>
#include <memory>
#include "Session.h"

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_context& io_context, short port);

private:
    void do_accept();

    tcp::acceptor acceptor_;
};