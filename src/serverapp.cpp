/**
 * @file serverapp.cpp
 * @author josh lyon (joshlyon@udel.edu)
 * @brief contains application code to run the server side of the chatroom program
 * @version 0.1
 */
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
    
}