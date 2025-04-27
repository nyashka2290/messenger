#include <iostream>
#include <string>
#include <utility>

class User {
public:
    void update_name(std::string& new_name);
    std::string get_name();
private:
    std::string name;
};

