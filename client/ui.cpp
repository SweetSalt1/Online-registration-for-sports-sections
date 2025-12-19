#include "ui.h"
#include <iostream>

void UI::MainMenu() {
    while (true) {
        if (!session_.IsLogged()) {
            MenuGuest();
        }
        else if (session_.GetRole() == "student") {
            MenuStudent();
        }
        else if (session_.GetRole() == "secretary") {
            MenuSecretary();
        }
    }
}

void UI::MenuGuest() {
    std::cout << "\n1) Register\n2) Login\nChoose: ";
    int x;
    std::cin >> x;

    if (x == 1) {
        std::string l, p;
        std::cout << "Login: ";
        std::cin >> l;
        std::cout << "Password: ";
        std::cin >> p;

        if (api_.Register(l, p)) {
            std::cout << "OK!\n";
        }
        else {
            std::cout << "Failed\n";
        }
    }
    if (x == 2) {
        std::string l, p;
        std::cout << "Login: ";
        std::cin >> l;
        std::cout << "Password: ";
        std::cin >> p;

        if (api_.Login(l, p)) {
            std::cout << "Logged!\n";
        }
        else {
            std::cout << "Incorrect\n";
        }
    }
}

void UI::MenuStudent() {
    std::cout << "\n1) Sections\n2) Send Application\nChoose: ";
    int x;
    std::cin >> x;

    if (x == 1) {
        auto s = api_.GetSections();
        for (auto& a : s.sections) {
            std::cout << a.id << ") " << a.name << " - " << a.description << "\n";
        }
    }

    if (x == 2) {
        int id;
        std::string text;
        std::cout << "Section ID: ";
        std::cin >> id;
        std::cout << "Text: ";
        std::cin.ignore();
        std::getline(std::cin, text);

        if (api_.SendApplication(id, text)) {
            std::cout << "Sent\n";
        }
        else {
            std::cout << "Failed\n";
        }
    }
}

void UI::MenuSecretary() {
    std::cout << "\n1) Queue\n2) Moderate\nChoose: ";
    int x;
    std::cin >> x;

    if (x == 1) {
        auto q = api_.GetQueue();
        for (auto& a : q.list) {
            std::cout << a.student_id << ") " << a.student_name << " -> "
                << a.section_name << " [" << a.status << "]\n";
        }
    }

    if (x == 2) {
        int id;
        int ok;
        std::cout << "Student ID: ";
        std::cin >> id;
        std::cout << "1 approve / 0 reject: ";
        std::cin >> ok;

        if (api_.Moderate(id, ok)) {
            std::cout << "Done\n";
        }
        else {
            std::cout << "Failed\n";
        }
    }
}
