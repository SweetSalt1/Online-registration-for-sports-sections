#include <string>
#include <vector>
#include <memory>
#include <ctime>

/////секция 
struct SectionInfo {
    int id;
    std::string name;
    std::string trainer;
    int slots;            ////// всего мест
    int occupied;         //// занято 
    std::string status;   //////// open/closed
    
};
/////заявка
struct ApplicationInfo {
    int id;
    int section_id;
    int student_id;
    std::string status; // pending, accepted, rejected, paid     ожидание, принято ,отклонено ,оплачено
    std::string created_at;/// время создания заявки(опционально)
};

struct UserProfile {
    int id;
    std::string name;
    std::string role; ////// студент или 
    std::string email;
};

///// чтобы отправлять и принимать json /////////////////////////////////////////////////////////////////////////////////////
class JSONTool {
public:
    static std::string packLogin(const std::string& user, const std::string& pass);/////делаем строку из логина и пароля
    static std::string packApply(int sectionId, const std::string& comment, const std::string& token);/////делает json с токеном, ид, коментарием
    static std::string packToken(const std::string& token);
    static UserProfile unpackUser(const std::string& jsonResp);
    static std::vector<SectionInfo> unpackSections(const std::string& jsonResp);
    static ApplicationInfo unpackApplicationResp(const std::string& jsonResp);
};

///////хранит текущую сессию   ,,,,, чтобы сервер понимал кто делает запрос
class SessionMgr {
private:
    std::string token_;     // токен сессии
    std::string role_;      // роль 
    int userId_;
    time_t expiresAt_;//// время истечения сессии
public:
    void saveSession(const std::string& token, const std::string& role, int uid, time_t exp);//// сохраняет поля вызывается после login
    void clearSession();////////// выйти       убирает токен
    bool isLogged() const;///// залогинен ли пользователь
    std::string getToken() const;///// если нужно прикрепить данные к запросу
    std::string getRole() const;
    int getUserId() const;
};

// //////общение с сервером
class ApiC {
private:
    std::string baseUrl; ///// адрес сервера
    SessionMgr* sess;    // ссылка на менеджер сессии( по сути чтобы брать токен)
public:
    ApiC(const std::string& base);////
    void login(const std::string& user, const std::string& pass);
    void registerUser(const std::string& name, const std::string& email, const std::string& pass);
    void getSections(); // запросит список секций
    void getSection(int sectionId); // подробная инфа по секции
    void sendApplication(int sectionId, const std::string& comment); // отправка заявки (требует token)
    void getApplicationsForSection(int sectionId); // для секретаря — получить очередь заявок
    void updateApplicationStatus(int applicationId, const std::string& newStatus); // accepted / rejected / paid
    
    void sendRawGet(const std::string& endpoint, const std::string& token = "");
    void sendRawPost(const std::string& endpoint, const std::string& body, const std::string& token = "");
};



////// Главное окно 
class MainW {
private:
    ApiC* api;
    SessionMgr session;
    // виджеты:
     //SecList* secList;
      // LogForm* logForm;
    // AppForm* appForm;
public:
    MainW();
    void init();           // инициализация GUI
    void showS();          // нажатие кнопки "список секций" 
    void openL();          // открыть окно логина/регистрации
    void openForm(int sId);// открыть форму подачи заявки 

    void onSectionsGot(const std::vector<SectionInfo>& secs);
    void onLoginOk(const std::string& token, const std::string& role, int uid);
    void onError(const std::string& msg);
};
// Форма входа / регистрации
class LogForm {
private:
    ApiC* api;
public:
    LogForm(ApiC* a);
    void show(); // показать форму
    void doLogin(const std::string& user, const std::string& pass); // вызывает api->login
    void doRegister(const std::string& name, const std::string& email, const std::string& pass); // api->registerUser
    
    void onLoginResp(const std::string& jsonResp);
};

// Виджет списка секций (общий для student + secretary)
class SecList {
private:
    ApiC* api;
    std::vector<SectionInfo> data;
public:
    SecList(ApiC* a);
    void load(); // вызывает api->getSections()
    void showList(); // отрисовка
    void onGotSections(const std::vector<SectionInfo>& secs);
    // при клике на секцию
    void openDetails(int sectionId); // вызывает api->getSection(sectionId)
};

// Форма заявки (открывается при выборе секции и нажатии "записаться")
class AppForm {
private:
    ApiC* api;
    int secId;
public:
    AppForm(ApiC* a, int sectionId);
    void fillAndSend(const std::string& name, const std::string& phone, const std::string& comment);
    void onApplyResp(const std::string& jsonResp);
};

// Виджет очереди заявок (для секретаря)
class SecQueue {
private:
    ApiC* api;
    int secId;
    std::vector<ApplicationInfo> queue;
public:
    SecQueue(ApiC* a, int sectionId);
    void loadQueue(); // api->getApplicationsForSection(secId)
    void showQueue();
    void acceptApp(int appId);   // api->updateApplicationStatus(appId, "accepted")
    void rejectApp(int appId);   // api->updateApplicationStatus(appId, "rejected")
    void markPaid(int appId);    // api->updateApplicationStatus(appId, "paid")
    void onQueueGot(const std::vector<ApplicationInfo>& apps);
};


