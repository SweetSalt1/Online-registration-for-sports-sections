#pragma once
#include <string>
#include "types.h"

class JSONTool {
public:
    static std::string PackRegister(const std::string& login, const std::string& password);
    static std::string PackLogin(const std::string& login, const std::string& password);
    static std::string PackApplication(const std::string& token, int section_id, const std::string& text);
    static std::string PackModeration(const std::string& token, int student_id, bool approve);
    static NotificationsResponse UnpackNotifications(const std::string& json);

    static bool UnpackSimpleSuccess(const std::string& json);
    static LoginResponse UnpackLoginResponse(const std::string& json);
    static SectionsResponse UnpackSections(const std::string& json);
    static QueueResponse UnpackQueue(const std::string& json);
};
