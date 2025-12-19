/*#include <iostream>
#include <string>
#include "json.h"
#include "types.h"

int main() {
    std::string response = R"({
        "success": true,
        "token": "abc123",
        "role": "student"
    })";

    LoginResponse lr = JSONTool::UnpackLoginResponse(response);

    if (!lr.success) {
        std::cout << "TEST FAILED: success == false\n";
        return 1;
    }

    if (lr.token != "abc123") {
        std::cout << "TEST FAILED: wrong token\n";
        return 1;
    }

    if (lr.role != "student") {
        std::cout << "TEST FAILED: wrong role\n";
        return 1;
    }

    std::cout << "TEST PASSED: UnpackLoginResponse\n";
    return 0;
}
*/