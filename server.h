#include <string>
#include <vector>
#include <memory>
#include <ctime>

struct SectionInfo {
    int id;
    std::string name;
    std::string trainer;
    int slots;
    int occupied;
    std::string status;

};

struct ApplicationInfo {
    int id;
    int section_id;
    int student_id;
    std::string status;
    std::string created_at;
};

struct UserProfile {
    int id;
    std::string name;
    std::string role;
    std::string email;
};

class JSONTool {
public:
    static std::string packLogin(const std::string& user, const std::string& pass);
    static std::string packApply(int sectionId, const std::string& comment, const std::string& token);
    static std::string packToken(const std::string& token);
    static UserProfile unpackUser(const std::string& jsonResp);
    static std::vector<SectionInfo> unpackSections(const std::string& jsonResp);
    static ApplicationInfo unpackApplicationResp(const std::string& jsonResp);
};

class SessionMgr {
private:
    std::string token_;
    std::string role_;
    int userId_;
    time_t expiresAt_;
public:
    void saveSession(const std::string& token, const std::string& role, int uid, time_t exp);
    void clearSession();
    bool isLogged() const;
    std::string getToken() const;
    std::string getRole() const;
    int getUserId() const;
};

class ApiC {
private:
    std::string baseUrl;
    SessionMgr* sess;
public:
    ApiC(const std::string& base);
    void login(const std::string& user, const std::string& pass);
    void registerUser(const std::string& name, const std::string& email, const std::string& pass);
    void getSections();
    void getSection(int sectionId);
    void sendApplication(int sectionId, const std::string& comment);
    void getApplicationsForSection(int sectionId);
    void updateApplicationStatus(int applicationId, const std::string& newStatus);

    void sendRawGet(const std::string& endpoint, const std::string& token = "");
    void sendRawPost(const std::string& endpoint, const std::string& body, const std::string& token = "");
};



class MainW {
private:
    ApiC* api;
    SessionMgr session;
public:
    MainW();
    void init();
    void showS();
    void openL();
    void openForm(int sId);

    void onSectionsGot(const std::vector<SectionInfo>& secs);
    void onLoginOk(const std::string& token, const std::string& role, int uid);
    void onError(const std::string& msg);
};

class LogForm {
private:
    ApiC* api;
public:
    LogForm(ApiC* a);
    void show();
    void doLogin(const std::string& user, const std::string& pass);
    void doRegister(const std::string& name, const std::string& email, const std::string& pass);

    void onLoginResp(const std::string& jsonResp);
};

class SecList {
private:
    ApiC* api;
    std::vector<SectionInfo> data;
public:
    SecList(ApiC* a);
    void load();
    void showList();
    void onGotSections(const std::vector<SectionInfo>& secs);
    void openDetails(int sectionId);
};

class AppForm {
private:
    ApiC* api;
    int secId;
public:
    AppForm(ApiC* a, int sectionId);
    void fillAndSend(const std::string& name, const std::string& phone, const std::string& comment);
    void onApplyResp(const std::string& jsonResp);
};

class SecQueue {
private:
    ApiC* api;
    int secId;
    std::vector<ApplicationInfo> queue;
public:
    SecQueue(ApiC* a, int sectionId);
    void loadQueue();
    void showQueue();
    void acceptApp(int appId);
    void rejectApp(int appId);
    void markPaid(int appId);
    void onQueueGot(const std::vector<ApplicationInfo>& apps);
};
