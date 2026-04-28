
#include "database.h"
#include <iostream>
#include <syslog.h>
Database::Database() {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        conn.reset(driver->connect("tcp://127.0.0.1:3306", "sport_user", "validate_Password1%"));
        conn->setSchema("sport_sections_db");
        syslog(LOG_INFO, "База данных успешно подключена");
        std::cout << "Database connected successfully" << std::endl;
    } catch (sql::SQLException& e) {
        syslog(LOG_ERR, "Ошибка подключения к БД: %s", e.what());
        std::cerr << "Database connection error: " << e.what() << std::endl;
        throw;
    }
}

sql::Connection* Database::getConnection() { return conn.get(); }
