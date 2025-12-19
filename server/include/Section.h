#include "ConnectBD.h"

class Section {
public:
    StartServer startserver;
    Section(int section_id, const std::string& name, const std::string& description,
        const std::vector<int>& student_ids)
        : section_id_(section_id),
        name_(name),
        description_(description),
        student_ids_(student_ids)
    {

    }

    bool ChangeDataSection(const std::string& name_, const std::string& description_);
    string GetSectionInfo(int section_id, const std::string* name, const std::string* description);
    bool AddStudent(const int& student_id, const int& section_id);
    bool DeleteStudent(const int& student_id, const int& section_id);

    void Unpack(const crow::json::rvalue& json);

    crow::json::wvalue Pack() const;

private:
    int section_id_;
    std::string name_;
    std::string description_;
    std::vector<int> student_ids_;
};