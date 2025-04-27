
#include "User.h"

void User::update_name(std::string& new_name)  {
    name = new_name;
}

std::string User::get_name(){
    return name;
}

void User::add_session(std::shared_ptr<Session>& session) {
    sessions.push_back(session);
}
