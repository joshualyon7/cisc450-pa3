/**
 * @file Client.hpp
 * @author josh lyon (joshlyon@udel.edu)
 * @brief contains prototypes and members for the Client class
 * @version 0.1
 * 
 */
#include "types.hpp"
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string>
#include "Packet.hpp"
#include <thread>
#include <mutex>
#include <list>

class Client {
    private:
        // members
        std::string username;
        std::list<std::string> users;
        sockaddr_in server;
        short seq = 0;
        int sock;
        std::thread listener;
        volatile bool running;
        
        // methods
        void populateUserList(std::string users);
        Packet receiveMessageRaw();
        void listenToServer();
        void displayMessage(std::string message, std::string receiver, bool priv);
        void addUser(std::string user);
        void removeUser(std::string user);
        int sendMessage(std::string msg, MessageType type); 
        void stopListening();

    public:
        Client();
        
        //mutators
        std::string getUsername();
        std::list<std::string> getUsers();
        sockaddr_in getServer();
        int getSock();
        
        int joinServer();
        int leaveServer();
        int connectToServer(std::string ip, int port, sa_family_t family, int connType);
        int sendPrivate(std::string input);
        int sendPublic(std::string msg);
        void printUsers();
        void printHelp();
        void listen();

        ~Client();
};
