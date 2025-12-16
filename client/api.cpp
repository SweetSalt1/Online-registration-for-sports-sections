#include "api.h"
#include "json.h"
#include <cpr/cpr.h>



std::string API::Post(const std::string& url, const std::string& body) {
    auto r = cpr::Post(
        cpr::Url{ url },
        cpr::Body{ body },
        cpr::Header{ {"Content-Type", "application/json"} }
    );

    if (r.error.code != cpr::ErrorCode::OK)
        return "{}"; 

    return r.text;
}

std::string API::Get(const std::string& url) {
    auto r = cpr::Get(
        cpr::Url{ url },
        cpr::Header{ {"Content-Type", "application/json"} }
    );

    if (r.error.code != cpr::ErrorCode::OK)
        return "{}";

    return r.text;
}



bool API::Register(const std::string& login, const std::string& password) {
    std::string body = JSONTool::PackRegister(login, password);

    std::string response = Post(
        BASE_URL + "/register",
        body
    );

    return JSONTool::UnpackSimpleSuccess(response);
}

bool API::Login(const std::string& login, const std::string& password) {
    std::string body = JSONTool::PackLogin(login, password);

    std::string response = Post(
        BASE_URL + "/login",
        body
    );

    auto lr = JSONTool::UnpackLoginResponse(response);

    if (lr.success) {
        session_.SetToken(lr.token);
        session_.SetRole(lr.role);
    }

    return lr.success;
}

SectionsResponse API::GetSections() {
    std::string url = BASE_URL + "/sections?token=" + session_.GetToken();
    return JSONTool::UnpackSections(Get(url));
}

bool API::SendApplication(int section_id, const std::string& text) {
    std::string body =
        JSONTool::PackApplication(session_.GetToken(), section_id, text);

    std::string response = Post(BASE_URL + "/application", body);

    return JSONTool::UnpackSimpleSuccess(response);
}

QueueResponse API::GetQueue() {
    std::string url = BASE_URL + "/queue?token=" + session_.GetToken();
    return JSONTool::UnpackQueue(Get(url));
}

bool API::Moderate(int student_id, bool approve) {
    std::string body =
        JSONTool::PackModeration(session_.GetToken(), student_id, approve);

    std::string response = Post(BASE_URL + "/moderate", body);

    return JSONTool::UnpackSimpleSuccess(response);
}
