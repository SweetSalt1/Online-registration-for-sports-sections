#pragma once
#include <string>
#include <vector>

struct SectionInfo {
    int id;
    std::string name;
    std::string description;
    std::string trainer;
    int max_students;
    int current_students;
};

struct SectionsResponse {
    std::vector<SectionInfo> sections;
};

struct LoginResponse {
    bool success = false;
    std::string token;
    std::string role;
    int user_id;
};

struct QueueItem {
    int application_id;    
    int student_id;
    std::string student_name;
    std::string section_name;
    std::string status;
    std::string text;
    std::string application_date;
};

struct QueueResponse {
    std::vector<QueueItem> list;
};