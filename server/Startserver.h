#pragma once

#include "Authentication.h"
#include "Student.h"
#include "Secretary.h"
#include "Session.h"
class StartServer
{
  public:
  Authentication auth("user","123","Secretary","321");
  Student student(1);

  crow::SimpleApp app;


  StartServer(int port_);

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
  int port;
}
