#include "Authentication.h"
class Authentication {
public:
    ConnectBD startserver;
    bool RegistrationUser(const std::string& login_, const std::string& password_, const std::string& role_)
    {
        try {
            auto conn = startserver.getConnection();
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement("INSERT INTO users (email, password, role ENUM, user_id) VALUES (?, ?, ?, ?)"));
            pstmt->setString(1, login_);
            pstmt->setString(2, password_);
            pstmt->setString(3, role_);
            pstmt->setString(4, student_id_);
            int rows = pstmt->executeUpdate();
            student_id_++;
            return rows>0;
        }
        catch (sql::SQLException& e) { CROW_LOG_ERROR << "MySQL Error: " << e.what(); return false; }
    }
    bool SignUser(const std::string& login_, const std::string& password_)
    {
        try {
            auto conn = startserver.getConnection();
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
                "SELECT password FROM users WHERE login = ?"));
            pstmt->setString(1, login_);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            if (res->next()) 
            { 
                std::string storedPassword = res->getString("password");
                return storedPassword == password_; 
            }
            else { return false; }
        }
        catch (sql::SQLException& e) { CROW_LOG_ERROR << "MySQL Error: " << e.what(); return false; }
    }

    bool ReplaceInfo(const std::string& login_, const std::string& password_, const std::string& new_password_)
    {
        try {
            auto conn = startserver.getConnection();
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement("SELECT password FROM users WHERE login = ?"));
            pstmt->setString(1, login_);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            if (res->next()) 
            { 
                std::string storedPassword = res->getString("password");
                if(storedPassword == password_)
                {
                    pstmt(conn->prepareStatement("UPDATE users SET password = ? WHERE login = ?"));
                    pstmt->setString(1, new_password_);
                    pstmt->setString(2, login_);
                    pstmt->executeUpdate();
                    return true;
                }
                else return false;
            }
            else return false;
        }
        catch (sql::SQLException& e) { CROW_LOG_ERROR << "MySQL Error: " << e.what(); return false; }
    }

    void Unpack(const crow::json::rvalue& json) {
        login_ = json["login"].s();
        password_ = json["password"].s();
        role_ = json["role"].s();
    }
    
    crow::json::wvalue Pack() const {
        crow::json::wvalue result;
        result["login"] = login_;
        result["password"] = password_;
        result["role"] = role_;
        return result;
    }

private:
    std::string login_;
    std::string password_;
    std::string role_;
    int student_id_=1;
};
