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