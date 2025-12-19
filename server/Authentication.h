#pragma once
#include "ConnectBD.h"

class Authentication {
public:
    ConnectBD startserver("tcp://127.0.0.1:3306","user","123Bd321!","mybd");
    Authentication(std::string& login, std::string& password,std::string& role,std::string& new_password);
    bool RegistrationUser(const std::string& login_, const std::string& password_, const std::string& role_);
    bool SignUser(const std::string& login_, const std::string& password_);

    bool ReplaceInfo(const std::string& login_, const std::string& password_, const std::string& new_password_);

    void Unpack(const crow::json::rvalue& json);
    
    crow::json::wvalue Pack() const;

private:
    std::string login_;
    std::string password_;
    std::string role_;
    std::string new_password;
    int student_id_=1;
};
