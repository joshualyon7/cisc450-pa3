#include "Server.hpp"
#include <iostream>
#include "config.hpp"
#include <unistd.h>


int main(void) {
    std::cout << "server starting up..." << std::endl;

    Server server;
    
    if(server.openRoom(SERVER_PORT, INADDR_ANY, AF_INET) < 0) {
        std::cout << "problem opening room " << std::endl;
        return -1;
    }
    
    server.acceptConnections();
    
    while(server.getRunning()) {
        sleep(10);

    }

    
}