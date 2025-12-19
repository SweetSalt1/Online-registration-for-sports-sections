class Authentication {
public:
    ConnectBD startserver;
    bool RegistrationUser(const std::string& login_, const std::string& password_, const std::string& role_);
    bool SignUser(const std::string& login_, const std::string& password_);

    bool ReplaceInfo(const std::string& login_, const std::string& password_, const std::string& new_password_);

    void Unpack(const crow::json::rvalue& json);
    
    crow::json::wvalue Pack() const;

private:
    std::string login_;
    std::string password_;
    std::string role_;
    int student_id_=1;
};