#include <string>
#include <map>
#include <thread>
#include "Packet.hpp"
#include "ClientInfo.hpp"


class Server {
    private:
        std::map<std::string, ClientInfo> connections;
        std::map<std::string, std::thread> listeners;
        std::thread welcomeThread;
        ClientInfo srvInfo;
        sockaddr_in info;
        int sock;
        int seq = 0;
        volatile bool running = true;
        
        int sendToOne(std::string message, ClientInfo sender, ClientInfo receiver);
        int sendToAll(std::string message, ClientInfo sender, MessageType type);
        bool userExists(std::string username);
        ClientInfo getUser(std::string username);
        void welcomeUser(Packet p, sockaddr_in *info);
        void removeUser(std::string username);
        void handleMessages();
        int sendMessage(std::string msg, MessageType type, ClientInfo sender, ClientInfo receiver);
    public:
        Server();
        
        int openRoom(int port, in_addr_t in_addr, sa_family_t family);
        void acceptConnections();
        void closeRoom();
        bool getRunning();

        ~Server();
};
