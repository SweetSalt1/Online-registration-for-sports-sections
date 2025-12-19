#include <crow.h>
#include <iostream>
#include <string>
#include <vector>
#include <Authentication.cpp>
#include <Student.cpp

class StartServer
{
  public:
  Authentication auth;
  Student student;

  crow::SimpleApp app;

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

    CROW_ROUTE(app, "/register").methods("POST"_method)
    ([](const crow::request& req) {
        try 
        {
            auto json = crow::json::load(req.body);
            
            if (!json || !json.has("login") || !json.has("password")) 
              {return crow::response(400, "Invalid JSON");}
            
            std::string login = json["login"].s();
            std::string password = json["password"].s();
            std::string role = json["role"].s();

            if (auth.RegistrationUser(login, password, role)) 
              {return crow::response(201, "User created"); } 
            else { return crow::response(409, "User exists"); }
        } 
        catch (const std::exception& e) 
            {return crow::response(500, "Server error");}
    });
    
    CROW_ROUTE(app, "/login").methods("POST"_method)
    ([](const crow::request& req) {
      try
      {
        auto json = crow::json::load(req.body);

        std::string login = json["login"].s();
        std::string password = json["password"].s();
        
        if (auth.SignUser(login, password)) 
        { return crow::response(200, "Login successful"); } 
        else { return crow::response(401, "Invalid credentials"); }
      }
      catch (const std::exception& e) 
            {return crow::response(500, "Server error");}
    });

    CROW_ROUTE(app, "/replace/password").methods("POST"_method)
    ([](const crow::request& req) {
        try 
        {
            auto json = crow::json::load(req.body);
            
            if (!json || !json.has("login") || !json.has("password")) 
              {return crow::response(400, "Invalid JSON");}
            
            std::string login = json["login"].s();
            std::string password = json["password"].s();
            std::string new_password = json["new_password"].s();

            if (auth.Replaceinfo(login, password, new_password)) 
              {return crow::response(201, "User created"); } 
            else { return crow::response(409, "User exists"); }
        } 
        catch (const std::exception& e) 
            {return crow::response(500, "Server error");}
    });

    CROW_ROUTE(app, "/insert/information/student").methods("POST"_method)
    ([](const crow::request& req) {
        try 
        {
            auto json = crow::json::load(req.body);

            std::string student_id = json["student_id"].s();
            std::string full_name = json["full_name"].s();
            std::string issued_by = json["issued_by"].s();
            std::string date_of_issue = json["date_of_issue"].s();

            if (student.InsertStudentInformation(student_id, full_name, issued_by, date_of_issue)) 
              {return crow::response(201, "Information inserted"); } 
            else { return crow::response(409, "Information did not insert"); }
        } 
        catch (const std::exception& e) 
            {return crow::response(500, "Server error");}
    });

    CROW_ROUTE(app, "/insert/application/student").methods("POST"_method)
    ([](const crow::request& req) {
        try 
        {
            auto json = crow::json::load(req.body);

            std::string student_id = json["student_id"].s();
            std::string section_id = json["section_id"].s();
            std::string status_application = json["status_application"].s();

            if (student.InsertStudentApplicatiion(student_id,section_id,status_application)) 
              {return crow::response(201, "Information inserted"); } 
            else { return crow::response(409, "Information did not insert"); }
        } 
        catch (const std::exception& e) 
            {return crow::response(500, "Server error");}
    });
    
    CROW_ROUTE(app, "/get/section/student").methods("POST"_method)
    ([](const crow::request& req) {
        try 
        {
            auto json = crow::json::load(req.body);

            std::string student_id = json["student_id"].s();

            std::string Mysection= student.MySection(student_id);
            return crow::response(201, Mysection); 
        } 
        catch (const std::exception& e) 
            {return crow::response(500, "Server error");}
    });

    CROW_ROUTE(app, "/get/myapplication/student").methods("POST"_method)
    ([](const crow::request& req) {
        try 
        {
            auto json = crow::json::load(req.body);

            std::string student_id = json["student_id"].s();

            std::string Myapplication= student.MySection(student_id);
            return crow::response(201, Myapplication); 
        } 
        catch (const std::exception& e) 
            {return crow::response(500, "Server error");}
    });

  void Start_server()
  {
    std::cout<< "Server running at http://localhost:8080" << std::endl;
    std::thread server_thread([](){ app.port(8080).multithreaded().run();});
  }


  private:
}
