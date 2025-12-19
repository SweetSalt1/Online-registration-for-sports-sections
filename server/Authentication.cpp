#include "Authentication.h"
#include "crow.h"
class Authentication {
public:
    Authentication(std::string& login, std::string& password,std::string& role,std::string& new_password)
    {
        login_=login;
        password_=password_;
        role_=role;
        new_password_=new_password;
    }
    bool RegistrationUser(const std::string& login, const std::string& password, const std::string& role)
    {
        try {
            auto conn = startserver.getConnection();
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
                "INSERT INTO users (email, password, role ENUM, user_id) VALUES (?, ?, ?, ?)"));
            pstmt->setString(1, login);
            pstmt->setString(2, password);
            pstmt->setString(3, role);
            pstmt->setString(4, student_id);
            int rows = pstmt->executeUpdate();
            student_id_++;
            return rows>0;
            delete pstmt;
            delete conn;
        }
        catch (sql::SQLException& e) { CROW_LOG_ERROR << "MySQL Error: " << e.what(); return false; }
    }
    bool SignUser(const std::string& login, const std::string& password)
    {
        try {
            auto conn = startserver.getConnection();
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
                "SELECT password FROM users WHERE email = ?"));
            pstmt->setString(1, login);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            if (res->next()) 
            { 
                std::string storedPassword = res->getString("password");
                return storedPassword == password; 
            }
            else { return false; }
            delete pstmt;
            delete conn;
        }
        catch (sql::SQLException& e) { CROW_LOG_ERROR << "MySQL Error: " << e.what(); return false; }
    }

    bool ReplaceInfo(const std::string& login, const std::string& password, const std::string& new_password)
    {
        try {
            auto conn = startserver.getConnection();
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
                "SELECT password FROM users WHERE email = ?"));
            pstmt->setString(1, login_);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            if (res->next()) 
            { 
                std::string storedPassword = res->getString("password");
                if(storedPassword == password)
                {
                    pstmt(conn->prepareStatement("UPDATE users SET password = ? WHERE email = ?"));
                    pstmt->setString(1, new_password);
                    pstmt->setString(2, login);
                    pstmt->executeUpdate();
                    return true;
                }
                else return false;
                delete pstmt;
                delete conn;
            }
            else return false;
        }
        catch (sql::SQLException& e) { CROW_LOG_ERROR << "MySQL Error: " << e.what(); return false; }
    }
    
    bool DeleteUser(const std::string&login)
    {
        try
        {
            auto conn = startserver.getConnection();
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
                "DELETE FROM users WHERE email = ?"));
            pstmt->setString(1, login);
            pstmt->executeUpdate();
            return true;
            delete pstmt;
            delete conn;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return false;
        }
        
    }

    crow::json::wvalue GetAllUsers()
    {
        try
        {
            auto conn = startserver.getConnection();
            sql::Statement* pstmt = conn->createStatement();
            sql::ResultSet* res = pstmt->executeQuery(
                "SELECT user_id, email, role FROM users WHERE is_deleted = 0");
            crow::json::wvalue response;
            response["success"] = true;
            
            int i = 0;
            while (res->next()) 
            {
                response["users"][i]["user_id"] = res->getInt("user_id");
                response["users"][i]["login"] = res->getString("email");
                response["users"][i]["role"] = res->getString("role");
                i++;
            }
            response["count"] = i;
            delete res;
            delete pstmt;
            delete conn;
            return response;
            
        } 
        catch (sql::SQLException& e) 
        {
            crow::json::wvalue error_response;
            error_response["success"] = false;
            error_response["error"] = e.what();
            return  error_response;
        }
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
    std::string new_password_;
    int student_id_=1;
};
