#include "Startserver.h"

int main() {
    ConnectBD startserver("tcp://127.0.0.1:3306","user","123Bd321!","mybd");
    Authentication auth("user","123","Secretary","321");
    StartServer start(8080);
    start.Start_server();
}
