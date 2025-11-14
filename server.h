#ifndef SERVER2_ARCHITECTURE_
#define SERVER2_ARCHITECTURE_
lku
#include <crow.h>

#include <list>
#include <memory>
#include <string>
#include <vector>

class Section;
class Student;
class Secretary;
class AuthenticationService;

class ActionsWithJSON {
 public:
  bool SendData(const std::list<Section>& sections_list);
  bool AcceptData(const std::string& login, const std::string& password);

  void Unpack(const crow::json::rvalue& json) {
    login_ = json["login"].s();
    password_ = json["password"].s();
    role_ = json["role"].s();
    registration_date_ = json["registration_date"].s();
    last_login_ = json["last_login"].s();
  }

  crow::json::wvalue Pack() const {
    crow::json::wvalue result;
    result["login"] = login_;
    result["password"] = password_;
    result["role"] = role_;
    result["registration_date"] = registration_date_;
    result["last_login"] = last_login_;
    return result;
  }
};

struct Session {
  std::string username;
  std::string role;
  time_t expires_at;
  std::list<Section> sections_list;
};

struct NotificationStudent {
  std::string name;
  std::string description;
};

class Authentication {
 public:
  bool RegistrationUser(const std::string& login, const std::string& password);
  std::unique_ptr<Session> SignIn();
  bool CheckSession();

 private:
  std::string login_;
  std::string password_;
  std::string role_;
};

class Section {
 public:
  Section(int section_id, const std::string& name, const std::string& description,
          const std::vector<int>& student_ids)
      : section_id_(section_id),
        name_(name),
        description_(description),
        student_ids_(student_ids) {}

  bool ChangeDataSection(int section_id, const std::string& name,
                         const std::string& description);
  bool GetSectionInfo(int section_id, std::string* name,
                     std::string* description) const;
  bool AddStudent(int student_id);
  bool DeleteStudent(int student_id);

 private:
  int section_id_;
  std::string name_;
  std::string description_;
  std::vector<int> student_ids_;
};

class Student {
 public:
  Student(int student_id, const std::string& name_student,
          const std::string& study_id, const std::string& issued_by,
          const std::string& date_of_issue)
      : student_id_(student_id),
        name_student_(name_student),
        study_id_(study_id),
        issued_by_(issued_by),
        date_of_issue_(date_of_issue) {}

  Student(int student_id, int section_id, const std::string& status_application,
          const std::vector<NotificationStudent>& notification,
          const std::string& sections_list, const std::string& status)
      : student_id_(student_id),
        section_id_(section_id),
        status_application_(status_application),
        notification_(notification),
        sections_list_(sections_list),
        status_(status) {}

  bool ChangeMyData(const std::string& name_student, const std::string& study_id,
                   const std::string& issued_by, const std::string& date_of_issue);
  bool MySection(const std::string& status);
  bool MyApplication(const std::string& form_data);
  bool ShowSectionsList(const std::list<Section>& sections_list) const;
  bool GetNotification(std::vector<NotificationStudent>* notification) const;

 private:
  int student_id_;
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

class Secretary {
 public:
  Secretary(const std::string& name, const std::string& applications_list)
      : name_(name), applications_list_(applications_list) {}

  bool ShowSectionsList(const std::list<Section>& sections_list,
                       const std::string& notification) const;
  bool ShowApplications(std::string* applications_list) const;
  bool GiveStatusFromSecretary(const std::string& form_data, const std::string& status,
                              std::vector<std::unique_ptr<Student>>* students);
  bool AddSectionToList(int section_id, const std::string& name,
                       const std::string& description);

 private:
  std::string name_;
  std::string status_;
  std::string applications_list_;
  std::string form_data_;
  std::vector<int> sections_ids_;
};

class DatabaseStudent {
 public:
  explicit DatabaseStudent(Database& database) : db_(database) {}
  bool SaveStudent(const Student& student);
  bool FindStudentInBD(int id);

 private:
  Database& db_;
};

class DatabaseSecretary {
 public:
  explicit DatabaseSecretary(Database& database) : db_(database) {}
  bool SaveSecretary(const Secretary& secretary);
  bool FindSecretaryInBD(int id);

 private:
  Database& db_;
};

class DatabaseSection {
 public:
  explicit DatabaseSection(Database& database) : db_(database) {}
  bool SaveSection(const Section& section);
  bool FindSectionInBD(int id);

 private:
  Database& db_;
};

class AuthenticationService {
 public:
  AuthenticationService(DatabaseStudent& student_repo,
                       DatabaseSecretary& secretary_repo)
      : student_repo_(student_repo), secretary_repo_(secretary_repo) {}

  bool RegisterUser(const std::string& login, const std::string& password,
                   const std::string& role);
  std::unique_ptr<Session> SignIn();

 private:
  DatabaseStudent& student_repo_;
  DatabaseSecretary& secretary_repo_;
};

#endif
