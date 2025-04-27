#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context; // Контекст ввода-вывода
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345)); // Слушаем порт 12345

        while (true) {
            tcp::socket socket(io_context); // Создаем сокет для клиента
            acceptor.accept(socket);        // Принимаем соединение
            std::cout << "Client connected!" << std::endl;

            char data[1024]; // Буфер для данных
            boost::system::error_code error;
            size_t length = socket.read_some(boost::asio::buffer(data), error); // Читаем данные
            if (error) {
                std::cerr << "Error reading: " << error.message() << std::endl;
                continue;
            }
            data[length] = '\0'; // Завершаем строку
            std::cout << "Received: " << data << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}