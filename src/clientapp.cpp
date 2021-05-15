#include "Client.hpp"
#include <iostream>
#include "config.hpp"

Command strToCommand(std::string cmd);

int main(void) {
    std::cout << "client starting up..." << std::endl;
    Client client;
    bool running = true;
    
    if(client.connectToServer(SERVER_ADDR, SERVER_PORT, AF_INET, SOCK_DGRAM) < 0) {
        std::cout << "error connecting to server" << std::endl;
        return -1;
    }
    
    client.joinServer();
    
    std::string input;
    while(running) {
        std::cout << "$>> ";
        std::cin >> input;

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
    
    std::cout << "Goodbye!" << std::endl;
    return 0;

}


Command strToCommand(std::string cmd) {
    if(cmd == "leave") return Command::LEAVE;
    if(cmd == "list") return Command::LIST;
    if(cmd == "private") return Command::PRIVATE;
    else return Command::HELP;
}