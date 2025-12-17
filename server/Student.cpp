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