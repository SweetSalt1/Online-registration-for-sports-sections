#include <crow.h>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include <mutex>
#include <queue>

#include <mysql/jdbc.h>
class StartServer()
{
  public:

  crow::SimpleApp app;


  std::unique_ptr<sql::Connection> getConnection()
  {
    driver = sql::mysql::get_mysql_driver_instance();
    std::unique_ptr<sql::Connection> conn(driver->connect(serverSQL, usernameSQL, passwordSQL));
    conn->setSchema(databaseSQL);
    return conn;
  }

  crow::response HandleJSONRequest(const crow::request& req) {
        auto json = crow::json::load(req.body);
        if (!json) {
            return crow::response(400, "Invalid JSON");
        }

        std::string action;
        if (json.has("action")) {
            action = json["action"].s();
        }

        if (action == "register") {
            std::string login = json["login"].s();
            std::string password = json["password"].s();
            std::string role = json["role"].s();

            crow::json::wvalue response;
            response["success"] = true;
            response["message"] = "User registered successfully";
            return crow::response(response);
        }

        return crow::response(404, "Action not found");
    }

  void Start_server()
  {
    std::cout<< "Server running at http://localhost:8080" << std::endl;
    std::thread server_thread([](){ app.port(8080).multithreaded().run();});
  }

  private:
  MYSQL* connection;
  sql::mysql::MySQL_Driver* driver;
  const std::string serverSQL = "tcp://127.0.0.1:3306";
  const std::string usernameSQL = "user";
  const std::string passwordSQL = "12345678";
  const std::string databaseSQL = "mybd";
}
