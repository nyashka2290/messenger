#include "Server.h"

Server::Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::cout << "Client connected!" << std::endl;
                    auto session = std::make_shared<Session>(std::move(socket));
                    User user = session->initialize();
                    add_session_with_user(session, user);
                } else {
                    std::cerr << "Accept error: " << ec.message() << std::endl;
                }
                do_accept();
            });
}

void Server::add_session_with_user(std::shared_ptr<Session>& session, User& user) {
    if (users.find(user.get_name()) == users.end()) {
        user.add_session(session);
        users.insert({user.get_name(), std::make_shared<User>(user)});
    } else {
        users[user.get_name()]->add_session(session);
    }
    session->link_user(users[user.get_name()]);
}
