#include "WorkWithJson.h"

std::string JSONTool::PackRegister(const std::string& login,
                                  const std::string& password) {
    crow::json::wvalue j;
    j["login"] = login;
    j["password"] = password;
    return crow::json::dump(j);
}

std::string JSONTool::PackLogin(const std::string& login,
                               const std::string& password) {
    crow::json::wvalue j;
    j["login"] = login;
    j["password"] = password;
    return crow::json::dump(j);
}

std::string JSONTool::PackApplication(const std::string& token,
                                      int section_id,
                                      const std::string& text) {
    crow::json::wvalue j;
    j["token"] = token;
    j["section_id"] = section_id;
    j["text"] = text;
    return crow::json::dump(j);
}

std::string JSONTool::PackModeration(const std::string& token,
                                     int student_id,
                                     bool approve) {
    crow::json::wvalue j;
    j["token"] = token;
    j["student_id"] = student_id;
    j["approve"] = approve;
    return crow::json::dump(j);
}

bool JSONTool::UnpackSimpleSuccess(const std::string& jsonStr) {
    try {
        auto j = crow::json::load(jsonStr);
        return j["success"].b();
    } catch (...) {
        return false;
    }
}

LoginResponse JSONTool::UnpackLoginResponse(const std::string& jsonStr) {
    LoginResponse r{};
    try {
        auto j = crow::json::load(jsonStr);
        r.success = j["success"].b();
        r.token = j["token"].s();
        r.role = j["role"].s();
    } catch (...) {
        r.success = false;
    }
    return r;
}

SectionsResponse JSONTool::UnpackSections(const std::string& jsonStr) {
    SectionsResponse resp;
    try {
        auto j = crow::json::load(jsonStr);
        if (j.has("sections")) {
            for (const auto& s : j["sections"]) {
                resp.sections.push_back({
                    s["id"].i(),
                    s["name"].s(),
                    s["description"].s()
                });
            }
        }
    } catch (...) {}
    return resp;
}

QueueResponse JSONTool::UnpackQueue(const std::string& jsonStr) {
    QueueResponse resp;
    try {
        auto j = crow::json::load(jsonStr);
        if (j.has("queue")) {
            for (const auto& q : j["queue"]) {
                resp.list.push_back({
                    q["student_id"].i(),
                    q["student_name"].s(),
                    q["section_name"].s(),
                    q["status"].s()
                });
            }
        }
    } catch (...) {}
    return resp;
}

void NotificationStudent::Unpack(const crow::json::rvalue& json) {
    name = json["name"].s();
    description = json["description"].s();
}

crow::json::wvalue NotificationStudent::Pack() const {
    crow::json::wvalue j;
    j["name"] = name;
    j["description"] = description;
    return j;
}
