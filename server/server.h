#ifndef SERVER_H
#define SERVER_H

#include "database.h"
#include "session_manager.h"
#include "auth_manager.h"
#include "crow_all.h"

class Server {
private:
    crow::SimpleApp app;
    Database db;
    SessionManager sessionManager;
    AuthManager authManager;

public:
    Server();
    void run(int port = 8081);

private:
    void setupRoutes();
};

#endif // SERVER_H
