// server.cpp
#include "server.h"
#include <iostream>
#include <syslog.h>

Server::Server() : db(), sessionManager(db), authManager(db) {
    openlog("SportServer", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Сервер запускается...");

    // Очищаем старые сессии при старте
    sessionManager.cleanupOldSessions();

    setupRoutes();

    syslog(LOG_INFO, "Сервер успешно инициализирован");
}

void Server::run(int port) {
    syslog(LOG_INFO, "Сервер запущен на порту %d", port);
    std::cout << "Starting server on port " << port << "..." << std::endl;
    app.port(port).multithreaded().run();
}

void Server::setupRoutes() {
    // Регистрация
    CROW_ROUTE(app, "/register").methods("POST"_method)([this](const crow::request& req) {
        syslog(LOG_INFO, "Получен запрос на регистрацию");

        auto json = crow::json::load(req.body);
        if (!json) {
            syslog(LOG_WARNING, "Некорректный JSON в запросе регистрации");
            return crow::response(400, "Invalid JSON");
        }

        std::string email = json["login"].s();
        std::string password = json["password"].s();
        std::string fullName = json["full_name"].s();

        bool success = authManager.registerUser(email, password, fullName);

        crow::json::wvalue response;
        response["success"] = success;
        response["message"] = success ? "Registration successful" : "User already exists";

        syslog(LOG_INFO, "Регистрация завершена: %s", success ? "успешно" : "неудача");
        return crow::response(response);
    });

    // Вход
    CROW_ROUTE(app, "/login").methods("POST"_method)([this](const crow::request& req) {
        syslog(LOG_INFO, "Получен запрос на вход");

        auto json = crow::json::load(req.body);
        if (!json) {
            syslog(LOG_WARNING, "Некорректный JSON в запросе входа");
            return crow::response(400, "Invalid JSON");
        }

        std::string email = json["login"].s();
        std::string password = json["password"].s();

        std::string token, role;
        int userId;

        bool success = authManager.loginUser(email, password, token, role, userId);

        crow::json::wvalue response;
        response["success"] = success;
        response["token"] = token;
        response["role"] = role;
        response["user_id"] = userId;

        syslog(LOG_INFO, "Вход завершен: %s", success ? "успешно" : "неудача");
        return crow::response(response);
    });

    // Список секций
    CROW_ROUTE(app, "/sections").methods("GET"_method)([this](const crow::request& req) {
        syslog(LOG_INFO, "Получен запрос на список секций");

        // Проверяем токен
        std::string token = req.url_params.get("token");
        if (token.empty()) {
            syslog(LOG_WARNING, "Запрос секций без токена");
            return crow::response(401, "Token required");
        }

        int userId;
        std::string role;
        if (!sessionManager.validateSession(token, userId, role)) {
            syslog(LOG_WARNING, "Недействительный токен при запросе секций");
            return crow::response(401, "Invalid or expired token");
        }

        try {
            std::unique_ptr<sql::PreparedStatement> pstmt(db.getConnection()->prepareStatement(
                "SELECT section_id, section_name, description, trainer, "
                "max_students, current_students FROM sections"
                ));
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

            std::vector<crow::json::wvalue> sections;
            while (res->next()) {
                crow::json::wvalue section;
                section["id"] = res->getInt("section_id");
                section["name"] = res->getString("section_name");
                section["description"] = res->getString("description");
                section["trainer"] = res->getString("trainer");
                section["max_students"] = res->getInt("max_students");
                section["current_students"] = res->getInt("current_students");
                sections.push_back(section);
            }

            crow::json::wvalue response;
            response["sections"] = std::move(sections);

            syslog(LOG_INFO, "Отправлен список секций (%d шт.) пользователю %d", sections.size(), userId);
            return crow::response(response);

        } catch (sql::SQLException& e) {
            syslog(LOG_ERR, "Ошибка получения секций: %s", e.what());
            std::cerr << "Error getting sections: " << e.what() << std::endl;
            return crow::response(500, "Database error");
        }
    });

    // Запись на секцию (подача заявки)
    CROW_ROUTE(app, "/application").methods("POST"_method)([this](const crow::request& req) {
        auto json = crow::json::load(req.body);
        if (!json) {
            syslog(LOG_WARNING, "Некорректный JSON в заявке");
            return crow::response(400, "Invalid JSON");
        }

        std::string token = json["token"].s();
        int sectionId = json["section_id"].i();
        std::string text = json["text"].s();

        // Проверяем токен и получаем ID пользователя
        int userId;
        std::string role;
        if (!sessionManager.validateSession(token, userId, role)) {
            syslog(LOG_WARNING, "Недействительный токен при подаче заявки");
            return crow::response(401, "Invalid or expired token");
        }

        // Проверяем, что пользователь студент
        if (role != "student") {
            syslog(LOG_WARNING, "Попытка подачи заявки не студентом (ID: %d)", userId);
            return crow::response(403, "Only students can apply");
        }

        // Проверяем, не подавал ли уже заявку на эту секцию
        try {
            std::unique_ptr<sql::PreparedStatement> checkStmt(db.getConnection()->prepareStatement(
                "SELECT application_id FROM applications "
                "WHERE student_id = ? AND section_id = ? AND status = 'pending'"
                ));
            checkStmt->setInt(1, userId);
            checkStmt->setInt(2, sectionId);
            std::unique_ptr<sql::ResultSet> res(checkStmt->executeQuery());

            if (res->next()) {
                syslog(LOG_WARNING, "Студент %d уже имеет pending заявку на секцию %d", userId, sectionId);
                crow::json::wvalue response;
                response["success"] = false;
                response["message"] = "You already have a pending application for this section";
                return crow::response(response);
            }

            // Создаем заявку
            std::unique_ptr<sql::PreparedStatement> pstmt(db.getConnection()->prepareStatement(
                "INSERT INTO applications (student_id, section_id, text, status) "
                "VALUES (?, ?, ?, 'pending')"
                ));
            pstmt->setInt(1, userId);
            pstmt->setInt(2, sectionId);
            pstmt->setString(3, text);
            pstmt->executeUpdate();

            syslog(LOG_INFO, "Создана заявка: студент %d -> секция %d", userId, sectionId);

            crow::json::wvalue response;
            response["success"] = true;
            response["message"] = "Application submitted successfully";

            return crow::response(response);

        } catch (sql::SQLException& e) {
            syslog(LOG_ERR, "Ошибка создания заявки: %s", e.what());
            std::cerr << "Error creating application: " << e.what() << std::endl;
            crow::json::wvalue response;
            response["success"] = false;
            response["message"] = "Database error";
            return crow::response(500, response);
        }
    });

    // Очередь заявок (для секретаря)
    CROW_ROUTE(app, "/queue").methods("GET"_method)([this](const crow::request& req) {
        std::string token = req.url_params.get("token");
        if (token.empty()) {
            syslog(LOG_WARNING, "Запрос очереди без токена");
            return crow::response(401, "Token required");
        }

        int userId;
        std::string role;
        if (!sessionManager.validateSession(token, userId, role)) {
            syslog(LOG_WARNING, "Недействительный токен при запросе очереди");
            return crow::response(401, "Invalid or expired token");
        }

        // Только секретарь может видеть очередь
        if (role != "secretary") {
            syslog(LOG_WARNING, "Попытка просмотра очереди не секретарем (ID: %d)", userId);
            return crow::response(403, "Access denied");
        }

        try {
            std::unique_ptr<sql::PreparedStatement> pstmt(db.getConnection()->prepareStatement(
                "SELECT a.application_id, a.student_id, u.full_name as student_name, "
                "s.section_name, a.status, a.text, a.application_date "
                "FROM applications a "
                "JOIN users u ON a.student_id = u.user_id "
                "JOIN sections s ON a.section_id = s.section_id "
                "ORDER BY a.application_date DESC"
                ));
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

            std::vector<crow::json::wvalue> queue;
            while (res->next()) {
                crow::json::wvalue item;
                item["application_id"] = res->getInt("application_id");
                item["student_id"] = res->getInt("student_id");
                item["student_name"] = res->getString("student_name");
                item["section_name"] = res->getString("section_name");
                item["status"] = res->getString("status");
                item["text"] = res->getString("text");
                item["application_date"] = res->getString("application_date");
                queue.push_back(item);
            }

            syslog(LOG_INFO, "Секретарь %d запросил очередь (%d заявок)", userId, queue.size());

            crow::json::wvalue response;
            response["queue"] = std::move(queue);
            return crow::response(response);

        } catch (sql::SQLException& e) {
            syslog(LOG_ERR, "Ошибка получения очереди: %s", e.what());
            std::cerr << "Error getting queue: " << e.what() << std::endl;
            return crow::response(500, "Database error");
        }
    });

    // Модерация заявок (для секретаря)
    CROW_ROUTE(app, "/moderate").methods("POST"_method)([this](const crow::request& req) {
        auto json = crow::json::load(req.body);
        if (!json) {
            syslog(LOG_WARNING, "Некорректный JSON в модерации");
            return crow::response(400, "Invalid JSON");
        }

        std::string token = json["token"].s();
        int applicationId = json["application_id"].i(); // Исправлено: теперь по ID заявки
        bool approve = json["approve"].b();

        // Проверяем токен
        int userId;
        std::string role;
        if (!sessionManager.validateSession(token, userId, role)) {
            syslog(LOG_WARNING, "Недействительный токен при модерации");
            return crow::response(401, "Invalid or expired token");
        }

        // Только секретарь может модерировать
        if (role != "secretary") {
            syslog(LOG_WARNING, "Попытка модерации не секретарем (ID: %d)", userId);
            return crow::response(403, "Access denied");
        }

        try {
            std::string newStatus = approve ? "accepted" : "rejected";
            std::string action = approve ? "одобрена" : "отклонена";

            // Получаем информацию о заявке
            std::unique_ptr<sql::PreparedStatement> getStmt(db.getConnection()->prepareStatement(
                "SELECT student_id, section_id FROM applications WHERE application_id = ?"
                ));
            getStmt->setInt(1, applicationId);
            std::unique_ptr<sql::ResultSet> res(getStmt->executeQuery());

            if (!res->next()) {
                crow::json::wvalue response;
                response["success"] = false;
                response["message"] = "Application not found";
                return crow::response(404, response);
            }

            int studentId = res->getInt("student_id");
            int sectionId = res->getInt("section_id");

            syslog(LOG_INFO, "Секретарь %d модерация заявки %d: %s", userId, applicationId, action.c_str());

            // Обновляем статус заявки
            std::unique_ptr<sql::PreparedStatement> pstmt(db.getConnection()->prepareStatement(
                "UPDATE applications SET status = ? WHERE application_id = ?"
                ));
            pstmt->setString(1, newStatus);
            pstmt->setInt(2, applicationId);
            int affectedRows = pstmt->executeUpdate();

            if (approve && affectedRows > 0) {
                // Проверяем, есть ли место в секции
                std::unique_ptr<sql::PreparedStatement> checkCapacityStmt(db.getConnection()->prepareStatement(
                    "SELECT current_students, max_students FROM sections WHERE section_id = ?"
                    ));
                checkCapacityStmt->setInt(1, sectionId);
                std::unique_ptr<sql::ResultSet> capacityRes(checkCapacityStmt->executeQuery());

                if (capacityRes->next()) {
                    int current = capacityRes->getInt("current_students");
                    int max = capacityRes->getInt("max_students");

                    if (current < max) {
                        // Обновляем количество студентов в секции
                        std::unique_ptr<sql::PreparedStatement> updateSectionStmt(db.getConnection()->prepareStatement(
                            "UPDATE sections SET current_students = current_students + 1 WHERE section_id = ?"
                            ));
                        updateSectionStmt->setInt(1, sectionId);
                        updateSectionStmt->executeUpdate();

                        // Записываем студента в секцию
                        std::unique_ptr<sql::PreparedStatement> updateUserStmt(db.getConnection()->prepareStatement(
                            "UPDATE users SET section_enrolled = ? WHERE user_id = ?"
                            ));
                        updateUserStmt->setInt(1, sectionId);
                        updateUserStmt->setInt(2, studentId);
                        updateUserStmt->executeUpdate();

                        syslog(LOG_INFO, "Студент %d записан в секцию %d", studentId, sectionId);
                    } else {
                        syslog(LOG_WARNING, "Нет мест в секции %d для студента %d", sectionId, studentId);
                    }
                }
            }

            crow::json::wvalue response;
            response["success"] = (affectedRows > 0);
            response["message"] = (affectedRows > 0) ? "Application moderated" : "Application not found";

            syslog(LOG_INFO, "Модерация завершена: %s", (affectedRows > 0) ? "успешно" : "заявка не найдена");
            return crow::response(response);

        } catch (sql::SQLException& e) {
            syslog(LOG_ERR, "Ошибка модерации: %s", e.what());
            std::cerr << "Error moderating application: " << e.what() << std::endl;
            crow::json::wvalue response;
            response["success"] = false;
            response["message"] = "Database error";
            return crow::response(500, response);
        }
    });

    CROW_ROUTE(app, "/health").methods("GET"_method)([]() {
        syslog(LOG_DEBUG, "Проверка здоровья сервера");
        return crow::response(200, "Server is running");
    });
}
