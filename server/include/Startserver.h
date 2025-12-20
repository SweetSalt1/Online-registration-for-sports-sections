#include <crow.h>
#include <iostream>
#include <string>
#include <vector>
#include "Authentication.h"
#include "Student.h"


class StartServer
{
  public:
  Authentication auth;
  Student student;

  crow::SimpleApp app;

  crow::response HandleJSONRequest(const crow::request& req);
      

    CROW_ROUTE(app, "/register").methods("POST"_method)
    ([](const crow::request& req) {   });
    
    CROW_ROUTE(app, "/login").methods("POST"_method)
    ([](const crow::request& req) { });

    CROW_ROUTE(app, "/replace/password").methods("POST"_method)
    ([](const crow::request& req) { });

    CROW_ROUTE(app, "/insert/information/student").methods("POST"_method)
    ([](const crow::request& req) {  });

    CROW_ROUTE(app, "/insert/application/student").methods("POST"_method)
    ([](const crow::request& req) {  });
    
    CROW_ROUTE(app, "/get/section/student").methods("POST"_method)
    ([](const crow::request& req) {   });

    CROW_ROUTE(app, "/get/myapplication/student").methods("POST"_method)
    ([](const crow::request& req) {  });

  void Start_server();

  private:
  std::string student;
}
