// api.cpp
#include "api.h"
#include "json.h"
#include <cpr/cpr.h>
#include <iostream>

// Базовый URL сервера
const std::string BASE_URL = "http://localhost:8080";

std::string API::Post(const std::string& endpoint, const std::string& body) {
    std::string url = BASE_URL + endpoint;

    std::cout << "POST " << url << std::endl;
    std::cout << "Body: " << body << std::endl;

    auto r = cpr::Post(
        cpr::Url{url},
        cpr::Body{body},
        cpr::Header{{"Content-Type", "application/json"}},
        cpr::Timeout{5000}
        );

    std::cout << "Response: " << r.status_code << " " << r.text << std::endl;

    if (r.error.code != cpr::ErrorCode::OK || r.status_code != 200) {
        std::cerr << "POST error: " << r.error.message << std::endl;
        return "{}";
    }

    return r.text;
}

std::string API::Get(const std::string& endpoint) {
    std::string url = BASE_URL + endpoint;

    std::cout << "GET " << url << std::endl;

    auto r = cpr::Get(
        cpr::Url{url},
        cpr::Header{{"Content-Type", "application/json"}},
        cpr::Timeout{5000}
        );

    std::cout << "Response: " << r.status_code << " " << r.text << std::endl;

    if (r.error.code != cpr::ErrorCode::OK || r.status_code != 200) {
        std::cerr << "GET error: " << r.error.message << std::endl;
        return "{}";
    }

    return r.text;
}

bool API::Register(const std::string& login, const std::string& password) {
    std::string body = JSONTool::PackRegister(login, password);
    std::string response = Post("/register", body);
    return JSONTool::UnpackSimpleSuccess(response);
}

bool API::Login(const std::string& login, const std::string& password) {
    std::string body = JSONTool::PackLogin(login, password);
    std::string response = Post("/login", body);

    auto lr = JSONTool::UnpackLoginResponse(response);

    if (lr.success) {
        session_.SetToken(lr.token);
        session_.SetRole(lr.role);
        std::cout << "Logged in as " << lr.role << std::endl;
    }

    return lr.success;
}

SectionsResponse API::GetSections() {
    std::string url = "/sections?token=" + session_.GetToken();
    return JSONTool::UnpackSections(Get(url));
}

bool API::SendApplication(int section_id, const std::string& text) {
    std::string body = JSONTool::PackApplication(session_.GetToken(), section_id, text);
    std::string response = Post("/application", body);
    return JSONTool::UnpackSimpleSuccess(response);
}

QueueResponse API::GetQueue() {
    std::string url = "/queue?token=" + session_.GetToken();
    return JSONTool::UnpackQueue(Get(url));
}

bool API::Moderate(int student_id, bool approve) {
    std::string body = JSONTool::PackModeration(session_.GetToken(), student_id, approve);
    std::string response = Post("/moderate", body);
    return JSONTool::UnpackSimpleSuccess(response);
}
