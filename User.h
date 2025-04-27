#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

class Session;

class User {
public:
    User() = default;
    explicit User(const std::string& new_name) : name(new_name) {}
    void update_name(std::string& new_name);
    std::string get_name() const;
    void add_session(std::shared_ptr<Session>& session);
    const std::vector<std::weak_ptr<Session>>& get_sessions() const;

private:
    std::string name = "guest";
    std::vector<std::weak_ptr<Session>> sessions;
};