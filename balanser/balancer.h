#include <crow.h>
#include <string>
#include <vector>
#include <list>
#include <utility>

struct LoadBalancerConfig {
    std::string algorithm_;
    int health_check_interval_;
    int session_timeout_;
    bool sticky_sessions_;
};

class LoadBalancer {
public:
    LoadBalancer(const LoadBalancerConfig& config);
    ~LoadBalancer();

    bool AddServer(const std::string& host, int port, int weight = 1);
    bool RemoveServer(const std::string& server_id);
    crow::response RouteRequest(const crow::request& req);
    void PrintStats();
    void StartBackendServers();

private:
    LoadBalancerConfig config_;
    std::vector<class ServerInstance*> backend_servers_;
    class LoadBalancingAlgorithm* algorithm_;
    std::vector<std::pair<std::string, std::string>> session_map_;
    bool running_;

    std::string GetClientIP(const crow::request& req);
    void ForwardRequest(const crow::request& req, crow::response& res, class ServerInstance* server);
    void HandleRegistration(const crow::request& req, crow::response& res, class ServerInstance* server);
    void HandleLogin(const crow::request& req, crow::response& res, class ServerInstance* server);
    void HandleReplacePassword(const crow::request& req, crow::response& res, class ServerInstance* server);
    void HandleInsertInfo(const crow::request& req, crow::response& res, class ServerInstance* server);
    void HandleInsertApplication(const crow::request& req, crow::response& res, class ServerInstance* server);
    void HandleGetSection(const crow::request& req, crow::response& res, class ServerInstance* server);
    void HandleGetApplication(const crow::request& req, crow::response& res, class ServerInstance* server);
    void CreateAlgorithm();
    std::string GetServerForSession(const std::string& session_id);
    void BindSessionToServer(const std::string& session_id, const std::string& server_id);
};
