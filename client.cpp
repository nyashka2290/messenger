#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context; // Создаем контекст ввода-вывода
        tcp::socket socket(io_context);     // Создаем сокет
        tcp::resolver resolver(io_context); // Резолвер для преобразования адреса
        auto endpoints = resolver.resolve("127.0.0.1", "12345"); // Локальный адрес и порт
        boost::asio::connect(socket, endpoints); // Подключаемся к серверу

        std::string message = "Hello, Server!";
        boost::asio::write(socket, boost::asio::buffer(message)); // Отправляем сообщение

        std::cout << "Sent: " << message << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}