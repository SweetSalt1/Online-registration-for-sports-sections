#include "session_manager.h"
#include <random>
#include <iostream>
#include <syslog.h>

SessionManager::SessionManager(Database& database) : db(database) {}

std::string SessionManager::generateSessionToken() {
    const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, chars.size() - 1);

    std::string token;
    for (int i = 0; i < 32; ++i) {
        token += chars[dist(generator)];
    }
    return token;
}

std::string SessionManager::createSession(int userId) {
    std::string token = generateSessionToken();

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(db.getConnection()->prepareStatement(
            "INSERT INTO user_sessions (session_id, user_id) VALUES (?, ?)"
        ));
        pstmt->setString(1, token);
        pstmt->setInt(2, userId);
        pstmt->executeUpdate();
        syslog(LOG_INFO, "Создана сессия для пользователя %d", userId);
        return token;
    } catch (sql::SQLException& e) {
        syslog(LOG_ERR, "Ошибка создания сессии: %s", e.what());
        std::cerr << "Error creating session: " << e.what() << std::endl;
        return "";
    }
}

bool SessionManager::validateSession(const std::string& token, int& userId, std::string& role) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(db.getConnection()->prepareStatement(
            "SELECT u.user_id, u.role FROM user_sessions s "
            "JOIN users u ON s.user_id = u.user_id "
            "WHERE s.session_id = ? AND s.created_at > DATE_SUB(NOW(), INTERVAL 7 DAY)"
        ));
        pstmt->setString(1, token);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            userId = res->getInt("user_id");
            role = res->getString("role");
            return true;
        }
        syslog(LOG_WARNING, "Недействительная или просроченная сессия");
        return false;
    } catch (sql::SQLException& e) {
        syslog(LOG_ERR, "Ошибка проверки сессии: %s", e.what());
        std::cerr << "Error validating session: " << e.what() << std::endl;
        return false;
    }
}

void SessionManager::cleanupOldSessions() {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(db.getConnection()->prepareStatement(
            "DELETE FROM user_sessions WHERE created_at < DATE_SUB(NOW(), INTERVAL 30 DAY)"
        ));
        int deleted = pstmt->executeUpdate();
        if (deleted > 0) {
            syslog(LOG_INFO, "Очищено %d старых сессий", deleted);
        }
    } catch (sql::SQLException& e) {
        syslog(LOG_ERR, "Ошибка очистки старых сессий: %s", e.what());
        std::cerr << "Error cleaning up old sessions: " << e.what() << std::endl;
    }
}
