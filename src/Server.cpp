#include "Server.hpp"
#include "pa.hpp"
#include "Packet.hpp"
#include "ClientInfo.hpp"
#include <arpa/inet.h>

Server::Server() {}

int Server::openRoom(int port, in_addr_t in_addr, sa_family_t family) {
    info.sin_family = family;
    info.sin_port = htons(port);
    info.sin_addr.s_addr = in_addr;
    
    srvInfo.setAddr(&info);
    srvInfo.setUsername("SERVER");

    if((sock = socket(family, SOCK_DGRAM, 0)) < 0) {
        perror("creating server sock failed with");
        return -1;
    }

    if(bind(sock, (sockaddr*)&info, sizeof(info)) < 0) {
        perror("binding server sock failed with");
        return -1;
    }
    
    return 1;

}

void Server::acceptConnections() {
    welcomeThread = std::thread(&Server::handleMessages, this);
    std::cout << "server accepting new connections..." << std::endl;
}

void Server::handleMessages() {
    Packet p;
    sockaddr_in *p_info;
    while(running) {
        p = rec_packet(sock, &p_info);
    std::cout << "rec IP: " << inet_ntoa(p_info->sin_addr) << std::endl;
        
        switch(p.getType()) {
            case MessageType::NEW_USER:
                welcomeUser(p, p_info);
                break;
            case MessageType::USER_EXIT:
                removeUser(p.getFrom());
                break;
            case MessageType::PUBLIC_CHAT:
                sendToAll(p.getMessage(), getUser(p.getFrom()), p.getType());
                break;
            case MessageType::PRIVATE_CHAT:
                break;
            default:
                break;
        }
    }
}

void Server::welcomeUser(Packet p, sockaddr_in *p_info) {
    ClientInfo ci(p.getFrom(), p_info);
    
    if(userExists(p.getFrom())) {
        sendMessage("USERNAME ALREADY EXISTS", MessageType::USER_REJECT, srvInfo, ci);
        return;
    }

    connections[p.getFrom()] = ci;
    std::string user_list = "";
    for(std::map<std::string, ClientInfo>::iterator user = connections.begin();
            user != connections.end(); user++) {
        user_list += user->first + ",";
    }
    
    sendMessage(user_list, MessageType::USER_ACCEPT, srvInfo, ci);
    sendToAll(ci.getUsername(), srvInfo, MessageType::NEW_USER);

}

int Server::sendMessage(std::string msg, MessageType type, ClientInfo sender, ClientInfo receiver) {
    Packet p(sender.getUsername(), msg, type, seq);
    
    if(send_packet(sock, *receiver.getInfo(), p) < 1) {
        std::cout << "problem sending message to " << receiver.getUsername() << std::endl;
        return -1;
    }
    return 1;
}

void Server::removeUser(std::string username) {
    connections.erase(username);
    sendToAll("USER " + username + " LEFT", srvInfo, MessageType::USER_EXIT);
}

void Server::closeRoom() {
    sendToAll("SERVER CLOSING", srvInfo, MessageType::SERVER_CLOSE);
}

int Server::sendToOne(std::string message, ClientInfo sender, ClientInfo receiver) {
    if(sendMessage(message, MessageType::PRIVATE_CHAT, sender, receiver) < 0) {
        std::cout << "problem sending message to user " << receiver.getUsername() << std::endl;
        return -1;
    }
    return 1;
}

int Server::sendToAll(std::string message, ClientInfo sender, MessageType type) {
    for(std::map<std::string, ClientInfo>::iterator it = connections.begin();
            it != connections.end(); it++) {
        if(sendMessage(message, MessageType::PUBLIC_CHAT, sender, it->second) < 0){
            std::cout << "problem sending message to user " << it->second.getUsername() << std::endl;
            return -1;
        }
    }
    return 1;
}

bool Server::userExists(std::string username) {
    return connections.find(username) != connections.end();
}

ClientInfo Server::getUser(std::string username) {
    return connections[username];
}

bool Server::getRunning() {
    return running;
}

Server::~Server() {}