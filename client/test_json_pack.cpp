/*#include <iostream>
#include <string>
#include "json.h"   

int main() {
    std::string login = "student1";
    std::string password = "12345";

    std::string json = JSONTool::PackLogin(login, password);

    if (json.find("student1") == std::string::npos) {
        std::cout << "TEST FAILED: login not found\n";
        return 1;
    }

    if (json.find("12345") == std::string::npos) {
        std::cout << "TEST FAILED: password not found\n";
        return 1;
    }

    std::cout << "TEST PASSED: PackLogin\n";
    return 0;
}
*/