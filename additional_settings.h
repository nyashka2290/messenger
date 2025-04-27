#pragma once

#include <termios.h>
#include <boost/asio.hpp>

void set_echo(bool enable) {
    struct termios tty{};
    tcgetattr(STDIN_FILENO, &tty);
    if (enable) {
        tty.c_lflag |= ECHO;
    } else {
        tty.c_lflag &= ~ECHO;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}