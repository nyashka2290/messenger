
#include "User.h"

void User::update_name(std::string& new_name)  {
    name = new_name;
}

std::string User::get_name(){
    return name;
}
