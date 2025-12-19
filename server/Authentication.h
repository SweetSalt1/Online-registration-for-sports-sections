#pragma once
#include "ConnectBD.h"

class Authentication {
public:
    ConnectBD startserver("tcp://127.0.0.1:3306","user","123Bd321!","mybd");
    Authentication(std::string& login, std::string& password,std::string& role,std::string& new_password);
    bool RegistrationUser(const std::string& login, const std::string& password, const std::string& role);
    bool SignUser(const std::string& login, const std::string& password);
    bool ReplaceInfo(const std::string& login, const std::string& password, const std::string& new_password);
    bool DeleteUser(const std::string&login);
    crow::json::wvalue GetAllUsers();
    void Unpack(const crow::json::rvalue& json);
    crow::json::wvalue Pack() const;
private:
    std::string login_;
    std::string password_;
    std::string role_;
    std::string new_password_;
    int student_id_=1;
};
