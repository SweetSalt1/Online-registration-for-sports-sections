#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include "database.h"
#include <string>

class AuthManager {
private:
    Database& db;

public:
    AuthManager(Database& database);
    bool registerUser(const std::string& email, const std::string& password,
                      const std::string& fullName, const std::string& role = "student");
    bool loginUser(const std::string& email, const std::string& password,
                   std::string& token, std::string& role, int& userId);
};

#endif
