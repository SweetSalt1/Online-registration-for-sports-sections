#pragma once
#include "api.h"
#include "session.h"

class UI {
public:
    UI(API& api, Session& session)
        : api_(api), session_(session) {}

    void MainMenu();

private:
    void MenuGuest();
    void MenuStudent();
    void MenuSecretary();

    API& api_;
    Session& session_;
};
