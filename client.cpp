#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <deque>
#include "additional_settings.h"

using boost::asio::ip::tcp;

// Глобальные переменные
boost::asio::io_context io_context;
boost::asio::ip::tcp::socket sock(io_context);
boost::asio::posix::stream_descriptor input(io_context, ::dup(STDIN_FILENO));
boost::asio::streambuf server_buffer;
boost::asio::streambuf console_buffer;
std::deque<std::string> write_msgs; // Очередь для отправки сообщений

std::string name;

void async_write_server();
// Логин: запрашивает имя и отправляет его серверу
void login() {
    while (true) {
        std::cout << "write your name" << std::endl;
        std::getline(std::cin, name);
        if (!name.empty()) {
            write_msgs.push_back(name + "\n");
            async_write_server(); // Начинаем отправку
            return;
        } else {
            std::cout << "try again" << std::endl;
        }
    }
}

// Асинхронное чтение от сервера
void async_read_server() {
    boost::asio::async_read_until(sock, server_buffer, '\n',
                                  [](const boost::system::error_code& error, std::size_t /*bytes_transferred*/) {
                                      if (!error) {
                                          std::string message;
                                          std::istream is(&server_buffer);
                                          std::getline(is, message);
                                          std::clog << "Server: " << message << std::endl;
                                          async_read_server(); // Продолжаем читать
                                      } else {
                                          if (error != boost::asio::error::eof && error != boost::asio::error::connection_reset) {
                                              std::cerr << "Server read error: " << error.message() << std::endl;
                                          }
                                          if (sock.is_open()) {
                                              sock.close();
                                          }
                                      }
                                  });
}

// Асинхронное чтение из консоли
void async_read_console() {
    boost::asio::async_read_until(input, console_buffer, '\n',
                                  [](const boost::system::error_code& error, std::size_t /*bytes_transferred*/) {
                                      if (!error) {
                                          std::string message;
                                          std::istream is(&console_buffer);
                                          std::getline(is, message);
                                          if (!message.empty()) {
                                              write_msgs.push_back(message + "\n");
                                              async_write_server(); // Отправляем сообщение
                                          }
                                          async_read_console(); // Продолжаем читать
                                      } else {
                                          if (error != boost::asio::error::eof) {
                                              std::cerr << "Console read error: " << error.message() << std::endl;
                                          }
                                      }
                                  });
}

// Асинхронная отправка сообщений серверу
void async_write_server() {
    if (write_msgs.empty() || !sock.is_open()) {
        return; // Нет сообщений или сокет закрыт
    }
    boost::asio::async_write(sock, boost::asio::buffer(write_msgs.front()),
                             [](const boost::system::error_code& error, std::size_t /*bytes_transferred*/) {
                                 if (!error) {
                                     write_msgs.pop_front(); // Удаляем отправленное сообщение
                                     async_write_server(); // Отправляем следующее, если есть
                                 } else {
                                     if (error != boost::asio::error::eof && error != boost::asio::error::connection_reset) {
                                         std::cerr << "Write error: " << error.message() << std::endl;
                                     }
                                     if (sock.is_open()) {
                                         sock.close();
                                     }
                                 }
                             });
}

int main() {
    set_echo(false); // отключаем эхо в консоли
    try {
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "12345");

        // Синхронное подключение
        boost::asio::connect(sock, endpoints);
        std::clog << "Connected to server!" << std::endl;

        // Запускаем логин и асинхронные операции
        login();
        if (sock.is_open()) {
            async_read_server();
            async_read_console();
        }

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    set_echo(true); // Восстанавливаем эхо перед выходом
    return 0;
}