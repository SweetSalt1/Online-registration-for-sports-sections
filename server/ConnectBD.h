#include <iostream>
#include <mysql/jdbc.h>
#include <mutex>
#include <queue>
#include <list>
#include <memory>
class ConnectBD
{
    public:
    std::unique_ptr<sql::Connection> getConnection();
    
    private:
    sql::mysql::MySQL_Driver* driver;
    const std::string serverSQL;
    const std::string usernameSQL;
    const std::string passwordSQL;
    const std::string databaseSQL;
}