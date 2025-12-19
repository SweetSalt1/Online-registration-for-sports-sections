#pragma once

#include <string>
#include <vector>
#include <list>
#include <ctime>
#include <crow/json.h>

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
    static std::string PackRegister(const std::string& login,
                                    const std::string& password);

    static std::string PackLogin(const std::string& login,
                                 const std::string& password);

    static std::string PackApplication(const std::string& token,
                                       int section_id,
                                       const std::string& text);

    static std::string PackModeration(const std::string& token,
                                      int student_id,
                                      bool approve);

    static bool UnpackSimpleSuccess(const std::string& jsonStr);

    static LoginResponse UnpackLoginResponse(const std::string& jsonStr);

    static SectionsResponse UnpackSections(const std::string& jsonStr);

    static QueueResponse UnpackQueue(const std::string& jsonStr);
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

    void Unpack(const crow::json::rvalue& json);
    crow::json::wvalue Pack() const;
};
