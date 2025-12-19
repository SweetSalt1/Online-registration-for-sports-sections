#include "ConnectBD.h"
class ConnectBD
{
    public:
    ConnectBD(const std::string& server, const std::string& username, 
              const std::string& password, const std::string& database)
        : serverSQL(server), usernameSQL(username), 
          passwordSQL(password), databaseSQL(database)
    {
        driver = sql::mysql::get_mysql_driver_instance();
    }
    std::unique_ptr<sql::Connection> getConnection()
    {
        std::unique_ptr<sql::Connection> conn(driver->connect(serverSQL_, usernameSQL_, passwordSQL_));
        conn->setSchema(databaseSQL_);
        return conn;
    }

    private:
    sql::mysql::MySQL_Driver* driver;
    const std::string serverSQL_ = "tcp://127.0.0.1:3306";
    const std::string usernameSQL_ = "user";
    const std::string passwordSQL_ = "123Bd321!";
    const std::string databaseSQL_ = "mybd";

    std::string serverSQL;
    std::string usernameSQL;
    std::string passwordSQL;
    std::string databaseSQL;
}
