/**
 * @file clientapp.cpp
 * @author josh lyon (joshlyon@udel.edu)
 * @brief contains application code to run the client side of the chatroom program
 * @version 0.1
 */
#include "Client.hpp"
#include <iostream>
#include "config.hpp"
#include <iomanip>
#include <string>

Command strToCommand(std::string cmd);

int main(void) {
    Client client;
    bool running = true;
    
    if(client.connectToServer(SERVER_ADDR, SERVER_PORT, AF_INET, SOCK_DGRAM) < 0) {
        std::cout << "error connecting to server" << std::endl;
        return -1;
    }
    
    
    client.joinServer();
    
    std::string input;
    char* in_str;
    while(running) {
        std::getline(std::cin, input);
        
        input = input.find_first_not_of(" ") == std::string::npos ? "" : input;
        if(input.length() == 0) continue;

        if(!(input[0] == '/')) {
            client.sendPublic(input);
            continue;
        }

        Command cmd = strToCommand(input.substr(1, input.find_first_of(' ') - 1));
        std::string toUser = "";

        switch(cmd) {
            case Command::HELP:
                client.printHelp();
                break;
            case Command::LIST:
                client.printUsers();
                break;
            case Command::LEAVE:
                client.leaveServer();
                running = 0;
                break;
            case Command::PRIVATE:
                client.sendPrivate(input);
                break;
            default:
                client.printHelp();
                break;
        }
    
    }
    
    return 0;

}

/**
 * @brief converts a user-entered command to a command of type Command
 * 
 * @param cmd 
 * @return Command 
 */
Command strToCommand(std::string cmd) {
    if(cmd == "leave") return Command::LEAVE;
    if(cmd == "list") return Command::LIST;
    if(cmd == "private") return Command::PRIVATE;
    else return Command::HELP;
}
