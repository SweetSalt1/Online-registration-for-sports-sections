#ifndef SERVER2_ARCHITECTURE_
#define SERVER2_ARCHITECTURE_
#include <crow.h>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include <mysql/mysql.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>

#include <mutex>
#include <queue>

#include "json.h"

struct SectionInfo {
    int id;
    std::string name;
    std::string description;
};

struct LoginResponse {
    bool success;
    std::string token;
    std::string role;
};

struct QueueItem {
    int student_id;
    std::string student_name;
    std::string section_name;
    std::string status;
};

struct QueueResponse {
    std::vector<QueueItem> list;
};

struct SectionsResponse {
    std::vector<SectionInfo> sections;
};

class JSONTool {
public:
    static std::string PackRegister(const std::string& login, const std::string& password) {
        crow::json::wvalue j;
        j["login"] = login;
        j["password"] = password;
        return crow::json::dump(j);
    }

    static std::string PackLogin(const std::string& login, const std::string& password) {
        crow::json::wvalue j;
        j["login"] = login;
        j["password"] = password;
        return crow::json::dump(j);
    }

    static std::string PackApplication(const std::string& token, int section_id, const std::string& text) {
        crow::json::wvalue j;
        j["token"] = token;
        j["section_id"] = section_id;
        j["text"] = text;
        return crow::json::dump(j);
    }

    static std::string PackModeration(const std::string& token, int student_id, bool approve) {
        crow::json::wvalue j;
        j["token"] = token;
        j["student_id"] = student_id;
        j["approve"] = approve;
        return crow::json::dump(j);
    }

    static bool UnpackSimpleSuccess(const std::string& jsonStr) {
        try {
            auto j = crow::json::load(jsonStr);
            return j["success"].b();
        }
        catch (...) {
            return false;
        }
    }

    static LoginResponse UnpackLoginResponse(const std::string& jsonStr) {
        LoginResponse r;
        try {
            auto j = crow::json::load(jsonStr);
            r.success = j["success"].b();
            r.token = j["token"].s();
            r.role = j["role"].s();
        }
        catch (...) {
            r.success = false;
            r.token = "";
            r.role = "";
        }
        return r;
    }

    static SectionsResponse UnpackSections(const std::string& jsonStr) {
        SectionsResponse resp;
        try {
            auto j = crow::json::load(jsonStr);

            if (j.has("sections")) {
                auto sections = j["sections"];
                for (size_t i = 0; i < sections.size(); i++) {
                    SectionInfo s;
                    s.id = sections[i]["id"].i();
                    s.name = sections[i]["name"].s();
                    s.description = sections[i]["description"].s();
                    resp.sections.push_back(s);
                }
            }
        }
        catch (...) {}
        return resp;
    }

    static QueueResponse UnpackQueue(const std::string& jsonStr) {
        QueueResponse resp;
        try {
            auto j = crow::json::load(jsonStr);

            if (j.has("queue")) {
                auto queue = j["queue"];
                for (size_t i = 0; i < queue.size(); i++) {
                    QueueItem q;
                    q.student_id = queue[i]["student_id"].i();
                    q.student_name = queue[i]["student_name"].s();
                    q.section_name = queue[i]["section_name"].s();
                    q.status = queue[i]["status"].s();
                    resp.list.push_back(q);
                }
            }
        }
        catch (...) {}
        return resp;
    }
};

class StarServer;
class Section;
class Student;
class Secretary;
class AuthenticationService;

class ActionsWithJSON {
public:
    bool SendData(const std::list<class Section>& sections_list);
    bool AcceptData(const std::string& login, const std::string& password, MYSQL* connection) {
        std::string query = "SELECT email, password FROM users WHERE email = '" + login + "'";
        bool res = false;

        if (mysql_query(connection, query.c_str()) == 0) {
            MYSQL_RES* result = mysql_store_result(connection);
            MYSQL_ROW row;

            if ((row = mysql_fetch_row(result))) {
                if (password == std::string(row[1] ? row[1] : "")) {
                    res = true;
                }
            }
            mysql_free_result(result);
        }
        return res;
    }

    void Unpack(const crow::json::rvalue& json) {
        std::string login__ = json["login"].s();
        std::string password__ = json["password"].s();
        std::string role__ = json["role"].s();
        std::string registration_date__ = json["registration_date"].s();
        std::string last_login__ = json["last_login"].s();
    }

    crow::json::wvalue Pack(const std::string& login_, const std::string& password_,
        const std::string& role_, const std::string& registration_date_,
        const std::string& last_login_) const {
        crow::json::wvalue result;
        result["login"] = login_;
        result["password"] = password_;
        result["role"] = role_;
        result["registration_date"] = registration_date_;
        result["last_login"] = last_login_;
        return result;
    }
};

struct Session {
    std::string username;
    std::string role;
    time_t expires_at;
    std::list<class Section> sections_list;
};

struct NotificationStudent {
    std::string name;
    std::string description;

    void Unpack(const crow::json::rvalue& json) {
        name = json["name"].s();
        description = json["description"].s();
    }

    crow::json::wvalue Pack() const {
        crow::json::wvalue result;
        result["name"] = name;
        result["description"] = description;
        return result;
    }
};

class Authentication {
public:
    StartServer startserver;
    bool RegistrationUser(const std::string& login_, const std::string& password_, const std::string& role_)
    {
        try {
            auto conn = startserver.getConnection();
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement("INSERT INTO users (login, password, role) VALUES (?, ?, ?)"));
            pstmt->setString(1, login_);
            pstmt->setString(2, password_);
            pstmt->setString(3, role_);
            return pstmt->executeUpdate() > 0;
        }
        catch (sql::SQLException& e) { CROW_LOG_ERROR << "MySQL Error: " << e.what(); return false; }
    }
    bool SignUser(const std::string& login_, const std::string& password_)
    {
        try {
            auto conn = startserver.getConnection();
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement("SELECT login and password FROM users WHERE VALUES = (?, ?)"));
            pstmt->setString(1, login_);
            pstmt->setString(2, password_);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            if (res->next()) { return true; }
            else { return false; }
        }
        catch (sql::SQLException& e) { return false; }
    }
    std::unique_ptr<Session> SignIn();
    bool CheckSession();
    bool ReplaceInfo(std::string* login__, std::string* password__,
        std::string* role__, std::string* registration_date__, std::string* last_login__)
    {
        login_ = login__;
        password_ = password__;
        role_ = role__;
        registration_date_ = registration_date__;
        last_login_ = last_login__;
    }

    void Unpack(const crow::json::rvalue& json) {
        login_ = json["login"].s();
        password_ = json["password"].s();
        role_ = json["role"].s();
        registration_date_ = json["registration_date"].s();
        last_login_ = json["last_login"].s();
    }
    crow::json::wvalue Pack() const {
        crow::json::wvalue result;
        result["login"] = login_;
        result["password"] = password_;
        result["role"] = role_;
        result["registration_date"] = registration_date_;
        result["last_login"] = last_login_;
        return result;
    }

private:
    std::string login_;
    std::string password_;
    std::string role_;
    std::string registration_date_;
    std::string last_login_;
};

class Section {
public:
    StartServer startserver;
    Section(int section_id, const std::string& name, const std::string& description,
        const std::vector<int>& student_ids)
        : section_id_(section_id),
        name_(name),
        description_(description),
        student_ids_(student_ids)
    {
    }

    bool ChangeDataSection(int section_id, const std::string& name, const std::string& description)
    {
        std::string query = "UPDATE sections SET description_ = '" + startserver.escapeString(description) + "' WHERE section_name = '" + startserver.escapeString(name) + "'";
        return mysql_query(connection, query.c_str()) == 0;
    }
    bool GetSectionInfo(int section_id, const std::string* name, const std::string* description)
    {
        std::string query = "SELECT description_ FROM sections WHERE section_id = '" + startserver.escapeString(section_id) + "'";
        if (mysql_query(connection, query.c_str()) == 0)
        {
            MYSQL_RES* result = mysql_store_result(connection);
            bool exists = (mysql_num_rows(result) > 0);
            mysql_free_result(result);
            return exists;
        }
    }
    bool AddStudent(const int& student_id, const int& section_id)
    {
        std::string query = "UPDATE users SET section_enrolled = '" + section_id.c_str() + "' WHERE user_id = '" + student_id.c_str() + "'";
        return mysql_query(connection, query.c_str()) == 0;
    }
    bool DeleteStudent(const int& student_id, const int& section_id)
    {
        std::string null = "0";
        std::string query = "UPDATE users SET section_enrolled = '" + null.c_str() + "' WHERE user_id = '" + student_id.c_str() + "'";
        return mysql_query(connection, query.c_str()) == 0;
    }

    void Unpack(const crow::json::rvalue& json) {
        section_id_ = json["section_id"].i();
        name_ = json["name"].s();
        description_ = json["description"].s();
        student_ids_.clear();
        auto student_ids_json = json["student_ids"];
        for (size_t i = 0; i < student_ids_json.size(); i++) {
            student_ids_.push_back(student_ids_json[i].i());
        }
    }

    crow::json::wvalue Pack() const {
        crow::json::wvalue result;
        result["section_id"] = section_id_;
        result["name"] = name_;
        result["description"] = description_;

        std::vector<crow::json::wvalue> student_ids_json;
        for (const auto& id : student_ids_) {
            student_ids_json.push_back(crow::json::wvalue(id));
        }
        result["student_ids"] = std::move(student_ids_json);

        return result;
    }

private:
    int section_id_;
    std::string name_;
    std::string description_;
    std::vector<int> student_ids_;
};

class Student {
public:
    StartServer startserver;
    Student(int student_id, const std::string& name_student,
        const std::string& study_id, const std::string& issued_by,
        const std::string& date_of_issue)
        : student_id_(student_id),
        name_student_(name_student),
        study_id_(study_id),
        issued_by_(issued_by),
        date_of_issue_(date_of_issue)
    {
        std::string query = "INSERT INTO users (student_id, full_name, issued_by, date_of_issue) VALUES ('" +
            startserver.escapeString(student_id_) + "', '" +
            startserver.escapeString(name_student_) + "', '" +
            startserver.escapeString(issued_by_) + "', '" +
            startserver.escapeString(date_of_issue_) + "')";
        return mysql_query(connection, query.c_str()) == 0;
    }
    Student(int student_id, int section_id, const std::string& status_application,
        const std::vector<NotificationStudent>& notification,
        const std::string& sections_list, const std::string& status)
        : student_id_(student_id),
        section_id_(section_id),
        status_application_(status_application),
        notification_(notification),
        sections_list_(sections_list),
        status_(status)
    {
        std::string query = "INSERT INTO applications (student_id, section_id, status ENUM) VALUES ('" +
            startserver.escapeString(student_id_) + "', '" +
            startserver.escapeString(section_id_) + "', '" +
            startserver.escapeString(status_application_) + "')";
        return mysql_query(connection, query.c_str()) == 0;
    }

    bool ChangeMyData(const std::string& name_student, const std::string& study_id,
        const std::string& issued_by, const std::string& date_of_issue,
        const std::string& new_name_student, const std::string& new_study_id,
        const std::string& new_issued_by, const std::string& new_date_of_issue)
    {
        query = "UPDATE users SET full_name = '" + startserver.escapeString(new_name_student) + "' WHERE full_name = '" + startserver.escapeString(name_student) + "'";
        mysql_query(connection, query.c_str());
        query = "UPDATE users SET user_id = '" + startserver.escapeString(new_study_id) + "' WHERE user_id = '" + startserver.escapeString(study_id) + "'";
        mysql_query(connection, query.c_str());
        query = "UPDATE users SET issued_by = '" + startserver.escapeString(issued_by) + "' WHERE issued_by = '" + startserver.escapeString(issued_by) + "'";
        mysql_query(connection, query.c_str());
        query = "UPDATE users SET date_of_issue = '" + startserver.escapeString(new_date_of_issue) + "' WHERE date_of_issue = '" + startserver.escapeString(date_of_issue) + "'";
        mysql_query(connection, query.c_str());
        return mysql_query(connection, query.c_str()) == 0;
    }
    bool MySection(const std::string& status, const int& student_id)
    {
        query = "SELECT status ENUM FROM users WHERE student_id = '" + student_id.c_str() + "'";
        if (mysql_query(connection, query.c_str()) == 0)
        {
            MYSQL_RES* result = mysql_store_result(connection);
            bool exists = (mysql_num_rows(result) > 0);
            mysql_free_result(result);
            return exists;
        }
    }
    bool MyApplication(const std::string& form_data, const int& student_id)
    {
        query = "SELECT application_id FROM applications WHERE student_id = '" + student_id.c_str() + "'";
        if (mysql_query(connection, query.c_str()) == 0)
        {
            MYSQL_RES* result = mysql_store_result(connection);
            bool exists = (mysql_num_rows(result) > 0);
            mysql_free_result(result);
            return exists;
        }
    }
    std::string ShowSectionsList(const std::list<Section>& sections_list) const
    {
        query = "SELECT section_name, trainer, description_, FROM sections";
        std::string result_json = "{\"sections\":[";
        if (mysql_query(connection, query.c_str()) == 0)
        {
            MYSQL_RES* result = mysql_store_result(connection);
            MYSQL_ROW row;
            bool first = true;

            while ((row = mysql_fetch_row(result)))
            {
                if (!first) result_json += ",";
                first = false;
                result_json += "{\"section_name\":" + std::string(row[0] ? row[0] : "0") +
                    ",\"trainer\":\"" + (row[1] ? row[1] : "") + "\"" +
                    ",\"description_\":\"" + (row[2] ? row[2] : "") + "\"}";
            }
            mysql_free_result(result);
        }
        result_json += "]}";
        return result_json;
    }
    bool GetNotification(std::vector<NotificationStudent>* notification) const;
    void Unpack(const crow::json::rvalue& json) {
        student_id_ = json["student_id"].i();
        name_student_ = json["name_student"].s();
        study_id_ = json["study_id"].s();
        issued_by_ = json["issued_by"].s();
        date_of_issue_ = json["date_of_issue"].s();
        section_id_ = json["section_id"].i();
        status_application_ = json["status_application"].s();

        notification_.clear();
        auto notification_json = json["notification"];
        for (size_t i = 0; i < notification_json.size(); i++) {
            NotificationStudent notif;
            notif.Unpack(notification_json[i]);
            notification_.push_back(notif);
        }

        sections_list_ = json["sections_list"].s();
        status_ = json["status"].s();
    }

    crow::json::wvalue Pack() const {
        crow::json::wvalue result;
        result["student_id"] = student_id_;
        result["name_student"] = name_student_;
        result["study_id"] = study_id_;
        result["issued_by"] = issued_by_;
        result["date_of_issue"] = date_of_issue_;
        result["section_id"] = section_id_;
        result["status_application"] = status_application_;

        std::vector<crow::json::wvalue> notification_json;
        for (const auto& notif : notification_) {
            notification_json.push_back(notif.Pack());
        }
        result["notification"] = std::move(notification_json);

        result["sections_list"] = sections_list_;
        result["status"] = status_;
        return result;
    }

private:
    std::string query;
    int student_id_;
    std::string name_student_;
    std::string study_id_;
    std::string issued_by_;
    std::string date_of_issue_;
    int section_id_ = -1;
    std::string status_application_ = "pending";
    std::vector<NotificationStudent> notification_;
    std::string sections_list_;
    std::string status_;
};

class Secretary {
public:
    StartServer startserver;
    Secretary(const std::string& name, const std::string& applications_list)
        : name_(name), applications_list_(applications_list) {
    }
    std::string ShowSectionsList(const std::list<Section>& sections_list, const std::string& notification)
    {
        query = "SELECT section_name, trainer, description_, FROM sections";
        std::string result_json = "{\"sections\":[";
        if (mysql_query(connection, query.c_str()) == 0)
        {
            MYSQL_RES* result = mysql_store_result(connection);
            MYSQL_ROW row;
            bool first = true;
            while ((row = mysql_fetch_row(result)))
            {
                if (!first) result_json += ",";
                first = false;
                result_json += "{\"section_name\":" + std::string(row[0] ? row[0] : "0") +
                    ",\"trainer\":\"" + (row[1] ? row[1] : "") + "\"" +
                    ",\"description_\":\"" + (row[2] ? row[2] : "") + "\"}";
            }
            mysql_free_result(result);
        }
        result_json += "]}";
        return result_json;
    }
    std::string ShowApplications(const std::string* applications_list)
    {
        query = "SELECT student_id, section_id, application_date, FROM applications";
        std::string result_json = "{\"sections\":[";
        if (mysql_query(connection, query.c_str()) == 0)
        {
            MYSQL_RES* result = mysql_store_result(connection);
            MYSQL_ROW row;
            bool first = true;
            while ((row = mysql_fetch_row(result)))
            {
                if (!first) result_json += ",";
                first = false;
                result_json += "{\"student_id\":" + std::string(row[0] ? row[0] : "0") +
                    ",\"section_id\":\"" + (row[1] ? row[1] : "") + "\"" +
                    ",\"section_id\":\"" + (row[2] ? row[2] : "") + "\"}";
            }
            mysql_free_result(result);
        }
        result_json += "]}";
        return result_json;
    }
    bool GiveStatusFromSecretary(const std::string& form_data, const std::string& status,
        std::vector<std::unique_ptr<Student>>* students);
    bool AddSectionToList(int section_id, const std::string& name, const std::string& description)
    {
        std::string query = "INSERT INTO sections (section_id, section_name, description_) VALUES ('" +
            startserver.escapeString(section_id) + "', '" +
            startserver.escapeString(name) + "', '" +
            startserver.escapeString(description) + "')";
        return mysql_query(connection, query.c_str()) == 0;
    }

    void Unpack(const crow::json::rvalue& json) {
        name_ = json["name"].s();
        applications_list_ = json["applications_list"].s();
        status_ = json["status"].s();
        form_data_ = json["form_data"].s();

        sections_ids_.clear();
        auto sections_ids_json = json["sections_ids"];
        for (size_t i = 0; i < sections_ids_json.size(); i++) {
            sections_ids_.push_back(sections_ids_json[i].i());
        }
    }

    crow::json::wvalue Pack() const {
        crow::json::wvalue result;
        result["name"] = name_;
        result["applications_list"] = applications_list_;
        result["status"] = status_;
        result["form_data"] = form_data_;

        std::vector<crow::json::wvalue> sections_ids_json;
        for (const auto& id : sections_ids_) {
            sections_ids_json.push_back(crow::json::wvalue(id));
        }
        result["sections_ids"] = std::move(sections_ids_json);

        return result;
    }

private:
    std::string query;
    std::string name_;
    std::string status_;
    std::string applications_list_;
    std::string form_data_;
    std::vector<int> sections_ids_;
};

class DatabaseStudent {
public:
    explicit DatabaseStudent(Database& database) : db_(database) {}
    bool SaveStudent(const Student& student);
    bool FindStudentInBD(int id);

private:
    Database& db_;
};

class DatabaseSecretary {
public:
    explicit DatabaseSecretary(Database& database) : db_(database) {}
    bool SaveSecretary(const Secretary& secretary);
    bool FindSecretaryInBD(int id);

private:
    Database& db_;
};

class DatabaseSection {
public:
    explicit DatabaseSection(Database& database) : db_(database) {}
    bool SaveSection(const Section& section);
    bool FindSectionInBD(int id);

private:
    Database& db_;
};

class AuthenticationService {
public:
    AuthenticationService(DatabaseStudent& student_repo,
        DatabaseSecretary& secretary_repo)
        : student_repo_(student_repo), secretary_repo_(secretary_repo) {
    }

    bool RegisterUser(const std::string& login, const std::string& password,
        const std::string& role);
    std::unique_ptr<Session> SignIn();

private:
    DatabaseStudent& student_repo_;
    DatabaseSecretary& secretary_repo_;
};

class StartServer {
public:
    MYSQL* connection;

    StartServer() : connection(nullptr) {
        connection = mysql_init(nullptr);
        if (!mysql_real_connect(connection, "localhost", "root", "", "myapp", 0, nullptr, 0)) {
            std::cerr << "Ошибка подключения: " << mysql_error(connection) << std::endl;
        }
    }

    std::string escapeString(const std::string& str) {
        char* escaped = new char[str.length() * 2 + 1];
        mysql_real_escape_string(connection, escaped, str.c_str(), str.length());
        std::string result(escaped);
        delete[] escaped;
        return result;
    }

    crow::response HandleJSONRequest(const crow::request& req) {
        auto json = crow::json::load(req.body);
        if (!json) {
            return crow::response(400, "Invalid JSON");
        }

        std::string action;
        if (json.has("action")) {
            action = json["action"].s();
        }

        if (action == "register") {
            std::string login = json["login"].s();
            std::string password = json["password"].s();
            std::string role = json["role"].s();

            crow::json::wvalue response;
            response["success"] = true;
            response["message"] = "User registered successfully";
            return crow::response(response);
        }

        return crow::response(404, "Action not found");
    }
};

#endif
