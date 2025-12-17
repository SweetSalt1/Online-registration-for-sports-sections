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
