#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include "database.h"
#include <string>

class SessionManager {
private:
    Database& db;

public:
    SessionManager(Database& database);
    std::string generateSessionToken();
    std::string createSession(int userId);
    bool validateSession(const std::string& token, int& userId, std::string& role);
    void cleanupOldSessions();
};

#endif
