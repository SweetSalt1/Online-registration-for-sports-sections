#include <iostream>
#include <string>
#include <vector>
#include <ctime>

class SimpleBalancer {
    std::vector<std::string> servers;
    int current;

public:
    SimpleBalancer() : current(0) {
        servers = {"localhost:8080", "localhost:8081", "localhost:8082"};
    }

    std::string getNext() {
        std::string server = servers[current];
        current = (current + 1) % servers.size();
        return server;
    }

    void run(int port = 8088) {
        std::cout << "порт: " << port << "\n\n";

        for(int i = 0; i < 10; i++) {
            std::cout << "Request " << i+1 << " - " << getNext() << "\n";
        }

        std::cout << "\n Балансировщик нагрузки запущен \n";
    }
};

int main() {
    SimpleBalancer balancer;
    balancer.run(8088);
    return 0;
}
