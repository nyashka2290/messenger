#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

class Session; // Предварительное объявление

class User {
public:
    void update_name(std::string& new_name);
    std::string get_name();
    void add_session(std::shared_ptr<Session>& session);
private:
    std::string name = "guest";
    std::vector<std::shared_ptr<Session>> sessions;
};