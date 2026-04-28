#include "ui.h"
#include <iostream>
#include <limits>

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void UI::MainMenu() {
    while (true) {
        if (!session_.IsLogged()) MenuGuest();
        else if (session_.GetRole() == "student") MenuStudent();
        else if (session_.GetRole() == "secretary") MenuSecretary();
        else {
            std::cout << "Unknown role. Logging out.\n";
            session_.SetToken("");
            session_.SetRole("");
        }
    }
}

void UI::MenuGuest() {
    int x = 0;
    std::cout << "\n=== SPORT SECTIONS ===\n";
    std::cout << "1) Register\n2) Login\n3) Exit\nChoose: ";
    std::cin >> x;
    clearInputBuffer();

    if (x == 1) {
        std::string l, p, fn;
        std::cout << "Login (email): "; std::getline(std::cin, l);
        std::cout << "Password: "; std::getline(std::cin, p);
        std::cout << "Full Name: "; std::getline(std::cin, fn);

        if (api_.Register(l, p, fn)) std::cout << "Registration successful!\n";
        else std::cout << "Registration failed (user may already exist)\n";
    }
    else if (x == 2) {
        std::string l, p;
        std::cout << "Login: "; std::getline(std::cin, l);
        std::cout << "Password: "; std::getline(std::cin, p);

        if (api_.Login(l, p)) std::cout << "Login successful!\n";
        else std::cout << "Incorrect login/password\n";
    }
    else if (x == 3) {
        std::cout << "Goodbye!\n";
        exit(0);
    }
}

void UI::MenuStudent() {
    int x = 0;
    std::cout << "\n=== STUDENT MENU ===\n";
    std::cout << "1) View Sections\n2) Send Application\n3) Logout\nChoose: ";
    std::cin >> x;
    clearInputBuffer();

    if (x == 1) {
        auto s = api_.GetSections();
        if (s.sections.empty()) {
            std::cout << "No sections available.\n";
            return;
        }

        std::cout << "\n=== AVAILABLE SECTIONS ===\n";
        for (auto& a : s.sections) {
            std::cout << "ID: " << a.id << "\n";
            std::cout << "  Name: " << a.name << "\n";
            std::cout << "  Description: " << a.description << "\n";
            std::cout << "  Trainer: " << a.trainer << "\n";
            std::cout << "  Students: " << a.current_students << "/" << a.max_students << "\n";
            std::cout << "--------------------------------\n";
        }
    }
    else if (x == 2) {
        int id; std::string text;
        std::cout << "Section ID: "; std::cin >> id;
        clearInputBuffer();
        std::cout << "Application text: "; std::getline(std::cin, text);

        if (api_.SendApplication(id, text)) std::cout << "Application sent successfully!\n";
        else std::cout << "Failed to send application (maybe you already applied)\n";
    }
    else if (x == 3) {
        session_.SetToken("");
        session_.SetRole("");
        std::cout << "Logged out.\n";
    }
}

void UI::MenuSecretary() {
    int x = 0;
    std::cout << "\n=== SECRETARY MENU ===\n";
    std::cout << "1) View Queue\n2) Moderate Application\n3) Logout\nChoose: ";
    std::cin >> x;
    clearInputBuffer();

    if (x == 1) {
        auto q = api_.GetQueue();
        if (q.list.empty()) {
            std::cout << "Queue is empty.\n";
            return;
        }

        std::cout << "\n=== APPLICATIONS QUEUE ===\n";
        for (auto& a : q.list) {
            std::cout << "App ID: " << a.application_id << "\n";
            std::cout << "  Student: " << a.student_name << " (ID: " << a.student_id << ")\n";
            std::cout << "  Section: " << a.section_name << "\n";
            std::cout << "  Status: " << a.status << "\n";
            std::cout << "  Text: " << a.text << "\n";
            std::cout << "  Date: " << a.application_date << "\n";
            std::cout << "--------------------------------\n";
        }
    }
    else if (x == 2) {
        int app_id, ok;
        std::cout << "Application ID (from queue): "; std::cin >> app_id;
        std::cout << "Action: 1-approve / 0-reject: "; std::cin >> ok;

        if (api_.Moderate(app_id, ok == 1)) {
            std::cout << "Moderation completed!\n";
        } else {
            std::cout << "Moderation failed (app not found or no places)\n";
        }
        clearInputBuffer();
    }
    else if (x == 3) {
        session_.SetToken("");
        session_.SetRole("");
        std::cout << "Logged out.\n";
    }
}
