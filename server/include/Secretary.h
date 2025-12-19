#include "Startserver.h"


class Secretary {
public:
    StartServer startserver;
    Secretary(const std::string& name, const std::string& applications_list)
        : name_(name), applications_list_(applications_list);
    std::string ShowSectionsList(const std::list<Section>& sections_list, const std::string& notification);
    std::string ShowApplications(const std::string* applications_list);
    bool GiveStatusFromSecretary(const std::string& form_data, const std::string& status,
        std::vector<std::unique_ptr<Student>>* students);
    bool AddSectionToList(int section_id, const std::string& name, const std::string& description);

    void Unpack(const crow::json::rvalue& json);

    crow::json::wvalue Pack() const;

private:
    std::string query;
    std::string name_;
    std::string status_;
    std::string applications_list_;
    std::string form_data_;
    std::vector<int> sections_ids_;
};