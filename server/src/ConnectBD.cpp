#include "ConnectBD.h"
class ConnectBD
{
    public:
    std::unique_ptr<sql::Connection> getConnection()
    {
        driver = sql::mysql::get_mysql_driver_instance();
        std::unique_ptr<sql::Connection> conn(driver->connect(serverSQL, usernameSQL, passwordSQL));
        conn->setSchema(databaseSQL);
        return conn;
    }

    private:
    sql::mysql::MySQL_Driver* driver;
    const std::string serverSQL = "tcp://127.0.0.1:3306";
    const std::string usernameSQL = "user";
    const std::string passwordSQL = "123Bd321!";
    const std::string databaseSQL = "mybd";
}