#pragma once
#include <string>
#include "types.h"
#include "session.h"

class API {
public:
    explicit API(Session& session) : session_(session) {}

    bool Register(const std::string& login, const std::string& password);
    bool Login(const std::string& login, const std::string& password);

    SectionsResponse GetSections();
    bool SendApplication(int section_id, const std::string& text);

    QueueResponse GetQueue();
    bool Moderate(int student_id, bool approve);

private:
    std::string Post(const std::string& url, const std::string& body);
    std::string Get(const std::string& url);
    const std::string BASE_URL = "http://АДРЕСС_СЕРВЕРА";

    Session& session_;
};
