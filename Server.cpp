#include "Server.h"

Server::Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    Session::set_users(users);
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::cout << "Client connected!" << std::endl;
                    auto session = std::make_shared<Session>(std::move(socket));
                    session->initialize([this, session](User user) {
                        add_session_with_user(session, user);
                        session->start();
                    });
                } else {
                    std::cerr << "Accept error: " << ec.message() << std::endl;
                }
                do_accept();
            });
}

void Server::add_session_with_user(std::shared_ptr<Session> session, User& user) {
    if (users.find(user.get_name()) == users.end()) {
        auto new_user = std::make_shared<User>(user);
        new_user->add_session(session);
        users.insert({user.get_name(), new_user});
    } else {
        users[user.get_name()]->add_session(session);
    }
    session->link_user(users[user.get_name()]);
}