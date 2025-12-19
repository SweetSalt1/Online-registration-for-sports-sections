#include "json.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// PACK

std::string JSONTool::PackRegister(const std::string& login,
    const std::string& password) {
    json j;
    j["login"] = login;
    j["password"] = password;
    return j.dump();
}

std::string JSONTool::PackLogin(const std::string& login,
    const std::string& password) {
    json j;
    j["login"] = login;
    j["password"] = password;
    return j.dump();
}

std::string JSONTool::PackApplication(const std::string& token, int section_id,
    const std::string& text) {
    json j;
    j["token"] = token;
    j["section_id"] = section_id;
    j["text"] = text;
    return j.dump();
}

std::string JSONTool::PackModeration(const std::string& token, int student_id,
    bool approve) {
    json j;
    j["token"] = token;
    j["student_id"] = student_id;
    j["approve"] = approve;
    return j.dump();
}

// UNPACK

bool JSONTool::UnpackSimpleSuccess(const std::string& json_str) {
    try {
        auto j = json::parse(json_str);
        return j.value("success", false);
    }
    catch (...) {
        return false;
    }
}

LoginResponse JSONTool::UnpackLoginResponse(const std::string& json_str) {
    LoginResponse r;

    try {
        auto j = json::parse(json_str);
        r.success = j.value("success", false);
        r.token = j.value("token", "");
        r.role = j.value("role", "");
    }
    catch (...) {
        r.success = false;
        r.token = "";
        r.role = "";
    }

    return r;
}

SectionsResponse JSONTool::UnpackSections(const std::string& json_str) {
    SectionsResponse resp;

    try {
        auto j = json::parse(json_str);

        if (!j.contains("sections") || !j["sections"].is_array()) return resp;

        for (auto& el : j["sections"]) {
            SectionInfo s;
            s.id = el.value("id", 0);
            s.name = el.value("name", "");
            s.description = el.value("description", "");
            resp.sections.push_back(s);
        }
    }
    catch (...) {
    }

    return resp;
}

QueueResponse JSONTool::UnpackQueue(const std::string& json_str) {
    QueueResponse resp;

    try {
        auto j = json::parse(json_str);

        if (!j.contains("queue") || !j["queue"].is_array()) return resp;

        for (auto& el : j["queue"]) {
            QueueItem q;
            q.student_id = el.value("student_id", 0);
            q.student_name = el.value("student_name", "");
            q.section_name = el.value("section_name", "");
            q.status = el.value("status", "");
            resp.list.push_back(q);
        }
    }
    catch (...) {
    }

    return resp;
}

NotificationsResponse JSONTool::UnpackNotifications(
    const std::string& json_str) {
    NotificationsResponse resp;

    try {
        auto j = nlohmann::json::parse(json_str);

        for (auto& el : j["notifications"]) {
            Notification n;
            n.name = el.value("name", "");
            n.description = el.value("description", "");
            n.timestamp = el.value("timestamp", "");

            resp.list.push_back(n);
        }
    }
    catch (...) {
    }

    return resp;
}
