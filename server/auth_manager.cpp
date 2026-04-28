
#include "auth_manager.h"
#include "session_manager.h"
#include <iostream>
#include <syslog.h>

AuthManager::AuthManager(Database& database) : db(database) {}

bool AuthManager::registerUser(const std::string& email, const std::string& password,
                               const std::string& fullName, const std::string& role) {
    try {
        syslog(LOG_INFO, "Попытка регистрации пользователя: %s", email.c_str());


        std::unique_ptr<sql::PreparedStatement> checkStmt(db.getConnection()->prepareStatement(
            "SELECT user_id FROM users WHERE email = ?"
        ));
        checkStmt->setString(1, email);
        std::unique_ptr<sql::ResultSet> res(checkStmt->executeQuery());

        if (res->next()) {
            syslog(LOG_WARNING, "Пользователь %s уже существует", email.c_str());
            return false;
        }


        std::unique_ptr<sql::PreparedStatement> pstmt(db.getConnection()->prepareStatement(
            "INSERT INTO users (email, password, full_name, role) VALUES (?, ?, ?, ?)"
        ));
        pstmt->setString(1, email);
        pstmt->setString(2, password);
        pstmt->setString(3, fullName);
        pstmt->setString(4, role);
        pstmt->executeUpdate();

        syslog(LOG_INFO, "Пользователь %s успешно зарегистрирован, роль: %s", email.c_str(), role.c_str());
        return true;
    } catch (sql::SQLException& e) {
        syslog(LOG_ERR, "Ошибка регистрации: %s", e.what());
        std::cerr << "Registration error: " << e.what() << std::endl;
        return false;
    }
}

bool AuthManager::loginUser(const std::string& email, const std::string& password,
                            std::string& token, std::string& role, int& userId) {
    try {
        syslog(LOG_INFO, "Попытка входа пользователя: %s", email.c_str());

        std::unique_ptr<sql::PreparedStatement> pstmt(db.getConnection()->prepareStatement(
            "SELECT user_id, password, role FROM users WHERE email = ?"
        ));
        pstmt->setString(1, email);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            std::string storedPassword = res->getString("password");


            if (password == storedPassword) {
                userId = res->getInt("user_id");
                role = res->getString("role");

                SessionManager sessionManager(db);
                token = sessionManager.createSession(userId);

                syslog(LOG_INFO, "Успешный вход: %s, роль: %s, ID: %d", email.c_str(), role.c_str(), userId);
                return !token.empty();
            }
        }
        syslog(LOG_WARNING, "Неудачная попытка входа для: %s", email.c_str());
        return false;
    } catch (sql::SQLException& e) {
        syslog(LOG_ERR, "Ошибка входа: %s", e.what());
        std::cerr << "Login error: " << e.what() << std::endl;
        return false;
    }
}
