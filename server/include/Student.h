#include "Student.h"

class Student {
public:
    ConnectBD startserver;

    bool InsertStudentInformation(int student_id, const std::string& name_student,
        const std::string& study_id, const std::string& issued_by,
        const std::string& date_of_issue);

    bool InsertStudentApplication(int student_id, int section_id, const std::string& status_application);

    bool ChangeMyData(const std::string& name_student, const std::string& study_id,
        const std::string& issued_by, const std::string& date_of_issue,
        const std::string& new_name_student, const std::string& new_study_id,
        const std::string& new_issued_by, const std::string& new_date_of_issue);

    std::string MySection(int student_id);

    std:string MyApplication(int student_id);
    std::string ShowSectionsList(const std::list<Section>& sections_list) const;

    void Unpack(const crow::json::rvalue& json);

    crow::json::wvalue Pack() const;

private:
    std::string query;
    int student_id_=1;
    std::string name_student_;
    std::string study_id_;
    std::string issued_by_;
    std::string date_of_issue_;
    int section_id_ = -1;
    std::string status_application_ = "pending";
    std::vector<NotificationStudent> notification_;
    std::string sections_list_;
    std::string status_;
};