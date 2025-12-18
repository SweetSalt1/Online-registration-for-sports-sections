#include <Startserver.cpp>

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

    bool ChangeDataSection(const std::string& name_, const std::string& description_)
    {
        try {
            auto conn = startserver.getConnection();
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
                "UPDATE sections SET description_ = ? WHERE section_name = ?"));
            pstmt->setString(1, name);
            pstmt->setString(2, description_);
            pstmt->executeUpdate();
            return true;
        }
        catch (sql::SQLException& e) { CROW_LOG_ERROR << "MySQL Error: " << e.what(); return false; }
    }
    string GetSectionInfo(int section_id, const std::string* name, const std::string* description)
    {
        try {
            auto conn = startserver.getConnection();
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
                "SELECT description_ FROM sections WHERE section_id = ?"));
            pstmt->setString(1, section_id);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            if (res->next()) 
            { 
                std::string descript = res->getString("description_");
                return descript; 
            }
            else { return false; }
        }
        catch (sql::SQLException& e) { CROW_LOG_ERROR << "MySQL Error: " << e.what(); return false; }
    }
    bool AddStudent(const int& student_id, const int& section_id)
    {
        try {
            auto conn = startserver.getConnection();
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
                "UPDATE users SET section_enrolled = ? WHERE user_id = ?"));
            pstmt->setString(1, section_id);
            pstmt->setString(2, student_id);
            pstmt->executeUpdate();
            return true;
        }
        catch (sql::SQLException& e) { CROW_LOG_ERROR << "MySQL Error: " << e.what(); return false; }
    }
    bool DeleteStudent(const int& student_id, const int& section_id)
    {
        try {
            auto conn = startserver.getConnection();
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
                "DELETE section_enrolled FROM users WHERE user_id = ?"));
            pstmt->setString(1, student_id);
            pstmt->executeUpdate();
            return true;
        }
        catch (sql::SQLException& e) { CROW_LOG_ERROR << "MySQL Error: " << e.what(); return false; }
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
