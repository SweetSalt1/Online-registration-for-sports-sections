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