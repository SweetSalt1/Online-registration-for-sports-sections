#pragma once

#include "Authentication.h"

//#include "Student.h"
//#include "Secretary.h"
//#include "Session.h"
class StartServer
{
  public:
StartServer(int port_);
void Start_server();
  private:
  int port;
}
