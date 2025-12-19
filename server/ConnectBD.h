#pragma once
#include <iostream>
#include <mysql/jdbc.h>
#include <mutex>
#include <queue>
#include <list>
#include <memory>
#include <string>
#include <vector>
class ConnectBD
{
    public:
    ConnectBD(const std::string& server, const std::string& username, 
              const std::string& password, const std::string& database)
        : serverSQL(server), usernameSQL(username), 
          passwordSQL(password), databaseSQL(database);
          
    std::unique_ptr<sql::Connection> getConnection();
    
    private:
    sql::mysql::MySQL_Driver* driver;
    const std::string serverSQL;
    const std::string usernameSQL;
    const std::string passwordSQL;
    const std::string databaseSQL;

    std::string serverSQL;
    std::string usernameSQL;
    std::string passwordSQL;
    std::string databaseSQL;
}
