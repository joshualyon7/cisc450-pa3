#include "Client.hpp"
#include <iostream>
#include "config.hpp"
#include <iomanip>
#include <ncurses.h>
#define OUTBOX_H 20

Command strToCommand(std::string cmd);
std::string processInput(std::string input);

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
        //std::cout << "before input" << std::endl;
        input = client.getDisplay().getInput();
        //std::cout << "got " + input << std::endl;
        
        input = input.find_first_not_of(" ") == std::string::npos ? "" : input;
        if(input.length() == 0) continue;
        


        if(!(input[0] == '/')) {
            client.sendPublic(input);
            continue;
        }

        Command cmd = strToCommand(input.substr(1, input.find_first_of(' ')));
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

Command strToCommand(std::string cmd) {
    if(cmd == "leave") return Command::LEAVE;
    if(cmd == "list") return Command::LIST;
    if(cmd == "private") return Command::PRIVATE;
    else return Command::HELP;
}

std::string processInput(std::string input) {
    if(input.find_first_not_of(" ") == std::string::npos) return "";
    return input;
}
