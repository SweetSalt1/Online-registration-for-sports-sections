#include <crow.h>
#include "Startserver.h"
#include <list>
#include <string>
#include <vector>

struct LoadBalancerConfig {
    std::string algorithm_;
    int health_check_interval_;
    int session_timeout_;
    bool sticky_sessions_;
};

struct ServerInstance {
    std::string id_;
    std::string host_;
    int port_;
    bool is_healthy_;
    int active_connections_;
    int weight_;
    StartServer* server_instance_;
    
    ServerInstance(const std::string& id, const std::string& host, int port, int weight = 1)
        : id_(id), host_(host), port_(port), is_healthy_(true),
          active_connections_(0), weight_(weight), server_instance_(nullptr) 
    {
        server_instance_ = new StartServer(port);
    }
    
    ~ServerInstance() {
        if (server_instance_) {
            delete server_instance_;
        }
    }
    
    bool CheckHealth() {
        return is_healthy_;
    }
    
    std::string GetUrl() const {
        return "http://" + host_ + ":" + std::to_string(port_);
    }
};

class LoadBalancingAlgorithm {
public:
    virtual ~LoadBalancingAlgorithm() = default;
    virtual ServerInstance* SelectServer(const std::string& client_ip = "") = 0;
    virtual void UpdateServerStats(const std::string& server_id, int connections) = 0;
    virtual void SetServers(const std::vector<ServerInstance*>& servers) = 0;
};

class RoundRobinAlgorithm : public LoadBalancingAlgorithm {
private:
    std::vector<ServerInstance*> servers_;
    size_t current_index_;
    
public:
    RoundRobinAlgorithm(const std::vector<ServerInstance*>& servers = {})
        : servers_(servers), current_index_(0) {}
    
    ServerInstance* SelectServer(const std::string& client_ip = "") override {
        if (servers_.empty()) return nullptr;
        
        size_t start = current_index_;
        do {
            if (servers_[current_index_]->is_healthy_) {
                ServerInstance* selected = servers_[current_index_];
                current_index_ = (current_index_ + 1) % servers_.size();
                return selected;
            }
            current_index_ = (current_index_ + 1) % servers_.size();
        } while (current_index_ != start);
        
        return nullptr;
    }
    
    void UpdateServerStats(const std::string& server_id, int connections) override {
        for (auto& server : servers_) {
            if (server->id_ == server_id) {
                server->active_connections_ = connections;
                break;
            }
        }
    }
    
    void SetServers(const std::vector<ServerInstance*>& servers) override {
        servers_ = servers;
        current_index_ = 0;
    }
};

class LoadBalancer {
private:
    LoadBalancerConfig config_;
    std::vector<ServerInstance*> backend_servers_;
    LoadBalancingAlgorithm* algorithm_;
    std::vector<std::pair<std::string, std::string>> session_map_;
    bool running_;
    
    std::string GetClientIP(const crow::request& req) {
        const char* x_real_ip = req.get_header_value("X-Real-IP");
        if (x_real_ip && x_real_ip[0] != '\0') {
            return std::string(x_real_ip);
        }
        
        const char* x_forwarded_for = req.get_header_value("X-Forwarded-For");
        if (x_forwarded_for && x_forwarded_for[0] != '\0') {
            std::string ip_list = x_forwarded_for;
            size_t comma_pos = ip_list.find(',');
            if (comma_pos != std::string::npos) {
                return ip_list.substr(0, comma_pos);
            }
            return ip_list;
        }
        
        return req.remote_ip_address;
    }
    
    void ForwardRequest(const crow::request& req, crow::response& res, ServerInstance* server) {
        crow::json::wvalue json_res;
        json_res["server"] = server->id_;
        json_res["host"] = server->host_;
        json_res["port"] = server->port_;
        
        std::string path = req.url;
        
        if (path == "/register") {
            return HandleRegistration(req, res, server);
        }
        else if (path == "/login") {
            return HandleLogin(req, res, server);
        }
        else if (path == "/replace/password") {
            return HandleReplacePassword(req, res, server);
        }
        else if (path == "/insert/information/student") {
            return HandleInsertInfo(req, res, server);
        }
        else if (path == "/insert/application/student") {
            return HandleInsertApplication(req, res, server);
        }
        else if (path == "/get/section/student") {
            return HandleGetSection(req, res, server);
        }
        else if (path == "/get/myapplication/student") {
            return HandleGetApplication(req, res, server);
        }
        
        res.body = crow::json::dump(json_res);
        res.code = 200;
        res.set_header("Content-Type", "application/json");
    }
    
    void HandleRegistration(const crow::request& req, crow::response& res, ServerInstance* server) {
        auto json = crow::json::load(req.body);
        
        if (!json || !json.has("login") || !json.has("password")) {
            res.code = 400;
            res.body = "Invalid JSON";
            return;
        }
        
        std::string login = json["login"].s();
        std::string password = json["password"].s();
        std::string role = json["role"].s();
        
        crow::json::wvalue response;
        response["server"] = server->id_;
        response["host"] = server->host_;
        response["port"] = server->port_;
        response["action"] = "register";
        response["login"] = login;
        
        res.body = crow::json::dump(response);
        res.code = 200;
        res.set_header("Content-Type", "application/json");
    }
    
    void HandleLogin(const crow::request& req, crow::response& res, ServerInstance* server) {
        auto json = crow::json::load(req.body);
        
        std::string login = json["login"].s();
        std::string password = json["password"].s();
        
        crow::json::wvalue response;
        response["server"] = server->id_;
        response["host"] = server->host_;
        response["port"] = server->port_;
        response["action"] = "login";
        response["login"] = login;
        
        res.body = crow::json::dump(response);
        res.code = 200;
        res.set_header("Content-Type", "application/json");
    }
    
    void HandleReplacePassword(const crow::request& req, crow::response& res, ServerInstance* server) {
        auto json = crow::json::load(req.body);
        
        if (!json || !json.has("login") || !json.has("password")) {
            res.code = 400;
            res.body = "Invalid JSON";
            return;
        }
        
        std::string login = json["login"].s();
        std::string password = json["password"].s();
        std::string new_password = json["new_password"].s();
        
        crow::json::wvalue response;
        response["server"] = server->id_;
        response["action"] = "replace_password";
        response["login"] = login;
        
        res.body = crow::json::dump(response);
        res.code = 200;
        res.set_header("Content-Type", "application/json");
    }
    
    void HandleInsertInfo(const crow::request& req, crow::response& res, ServerInstance* server) {
        auto json = crow::json::load(req.body);
        
        std::string student_id = json["student_id"].s();
        std::string full_name = json["full_name"].s();
        std::string issued_by = json["issued_by"].s();
        std::string date_of_issue = json["date_of_issue"].s();
        
        crow::json::wvalue response;
        response["server"] = server->id_;
        response["action"] = "insert_info";
        response["student_id"] = student_id;
        
        res.body = crow::json::dump(response);
        res.code = 200;
        res.set_header("Content-Type", "application/json");
    }
    
    void HandleInsertApplication(const crow::request& req, crow::response& res, ServerInstance* server) {
        auto json = crow::json::load(req.body);
        
        std::string student_id = json["student_id"].s();
        std::string section_id = json["section_id"].s();
        std::string status_application = json["status_application"].s();
        
        crow::json::wvalue response;
        response["server"] = server->id_;
        response["action"] = "insert_application";
        response["student_id"] = student_id;
        
        res.body = crow::json::dump(response);
        res.code = 200;
        res.set_header("Content-Type", "application/json");
    }
    
    void HandleGetSection(const crow::request& req, crow::response& res, ServerInstance* server) {
        auto json = crow::json::load(req.body);
        
        std::string student_id = json["student_id"].s();
        
        crow::json::wvalue response;
        response["server"] = server->id_;
        response["action"] = "get_section";
        response["student_id"] = student_id;
        response["section"] = "section_data_from_server";
        
        res.body = crow::json::dump(response);
        res.code = 200;
        res.set_header("Content-Type", "application/json");
    }
    
    void HandleGetApplication(const crow::request& req, crow::response& res, ServerInstance* server) {
        auto json = crow::json::load(req.body);
        
        std::string student_id = json["student_id"].s();
        
        crow::json::wvalue response;
        response["server"] = server->id_;
        response["action"] = "get_application";
        response["student_id"] = student_id;
        response["application"] = "application_data_from_server";
        
        res.body = crow::json::dump(response);
        res.code = 200;
        res.set_header("Content-Type", "application/json");
    }
    
    void CreateAlgorithm() {
        std::vector<ServerInstance*> server_ptrs;
        for (auto server : backend_servers_) {
            server_ptrs.push_back(server);
        }
        algorithm_ = new RoundRobinAlgorithm(server_ptrs);
    }
    
public:
    LoadBalancer(const LoadBalancerConfig& config)
        : config_(config), running_(false), algorithm_(nullptr) {
        CreateAlgorithm();
    }
    
    ~LoadBalancer() {
        for (auto server : backend_servers_) {
            delete server;
        }
        if (algorithm_) {
            delete algorithm_;
        }
    }
    
    bool AddServer(const std::string& host, int port, int weight = 1) {
        std::string id = host + ":" + std::to_string(port);
        
        for (auto server : backend_servers_) {
            if (server->id_ == id) {
                return false;
            }
        }
        
        ServerInstance* new_server = new ServerInstance(id, host, port, weight);
        backend_servers_.push_back(new_server);
        CreateAlgorithm();
        return true;
    }
    
    bool RemoveServer(const std::string& server_id) {
        for (auto it = backend_servers_.begin(); it != backend_servers_.end(); ++it) {
            if ((*it)->id_ == server_id) {
                delete *it;
                backend_servers_.erase(it);
                CreateAlgorithm();
                return true;
            }
        }
        return false;
    }
    
    crow::response RouteRequest(const crow::request& req) {
        crow::response res;
        
        std::string session_id;
        auto cookies_header = req.get_header_value("Cookie");
        if (!cookies_header.empty()) {
            size_t session_pos = cookies_header.find("session_id=");
            if (session_pos != std::string::npos) {
                size_t start = session_pos + 11;
                size_t end = cookies_header.find(';', start);
                if (end == std::string::npos) {
                    session_id = cookies_header.substr(start);
                } else {
                    session_id = cookies_header.substr(start, end - start);
                }
            }
        }
        
        ServerInstance* selected_server = nullptr;
        
        if (config_.sticky_sessions_ && !session_id.empty()) {
            std::string server_id = GetServerForSession(session_id);
            if (!server_id.empty()) {
                for (auto server : backend_servers_) {
                    if (server->id_ == server_id && server->is_healthy_) {
                        selected_server = server;
                        break;
                    }
                }
            }
        }
        
        if (!selected_server) {
            std::string client_ip = GetClientIP(req);
            selected_server = algorithm_->SelectServer(client_ip);
            
            if (selected_server && config_.sticky_sessions_ && !session_id.empty()) {
                BindSessionToServer(session_id, selected_server->id_);
            }
        }
        
        if (selected_server) {
            selected_server->active_connections_++;
            ForwardRequest(req, res, selected_server);
            selected_server->active_connections_--;
        } else {
            res.code = 503;
            res.body = "No available servers";
        }
        
        return res;
    }
    
    std::string GetServerForSession(const std::string& session_id) {
        for (const auto& session : session_map_) {
            if (session.first == session_id) {
                return session.second;
            }
        }
        return "";
    }
    
    void BindSessionToServer(const std::string& session_id, const std::string& server_id) {
        for (auto& session : session_map_) {
            if (session.first == session_id) {
                session.second = server_id;
                return;
            }
        }
        session_map_.push_back({session_id, server_id});
    }
    
    void PrintStats() {
        crow::json::wvalue json_stats;
        json_stats["algorithm"] = config_.algorithm_;
        json_stats["sticky_sessions"] = config_.sticky_sessions_;
        
        crow::json::wvalue::list servers_list;
        for (auto server : backend_servers_) {
            crow::json::wvalue server_info;
            server_info["id"] = server->id_;
            server_info["host"] = server->host_;
            server_info["port"] = server->port_;
            server_info["healthy"] = server->is_healthy_;
            server_info["connections"] = server->active_connections_;
            server_info["weight"] = server->weight_;
            servers_list.push_back(server_info);
        }
        json_stats["servers"] = std::move(servers_list);
        
        CROW_LOG_INFO << "Load Balancer Stats: " << crow::json::dump(json_stats);
    }
    
    void StartBackendServers() {
        for (auto server : backend_servers_) {
            if (server->server_instance_) {
            }
        }
    }
};

class SessionAwareMiddleware {
private:
    LoadBalancer& load_balancer_;
    
public:
    SessionAwareMiddleware(LoadBalancer& lb) : load_balancer_(lb) {}
    
    struct context {};
    
    void before_handle(crow::request& req, crow::response& res, context& ctx) {
    }
    
    void after_handle(crow::request& req, crow::response& res, context& ctx) {
    }
};
