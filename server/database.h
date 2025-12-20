// database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <memory>

class Database {
private:
    sql::mysql::MySQL_Driver* driver;
    std::unique_ptr<sql::Connection> conn;

public:
    Database();
    sql::Connection* getConnection();
};

#endif // DATABASE_H
