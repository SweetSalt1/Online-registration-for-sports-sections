#include "server.h"
#include <iostream>
#include <syslog.h>

int main() {
    openlog("SportServer", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Запуск приложения SportServer");

    try {
        Server server;
        syslog(LOG_INFO, "Сервер запущен на порту 8080");
        server.run(8080);
    } catch (const std::exception& e) {
        syslog(LOG_ERR, "Критическая ошибка сервера: %s", e.what());
        std::cerr << "Server error: " << e.what() << std::endl;
        closelog();
        return 1;
    }

    syslog(LOG_INFO, "Сервер завершает работу");
    closelog();
    return 0;
}
