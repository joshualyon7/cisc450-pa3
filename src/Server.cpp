/**
 * @file Server.cpp
 * @author josh lyon (joshlyon@udel.edu)
 * @brief contains implementations from methods in the Server class
 * @version 0.1
 */
#include "Server.hpp"
#include "pa.hpp"
#include "Packet.hpp"
#include "ClientInfo.hpp"
#include <arpa/inet.h>

Server::Server() {}

/**
 * @brief stets up socket to get ready to accept connections
 * 
 * @param port 
 * @param in_addr 
 * @param family 
 * @return int 
 */
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

/**
 * @brief starts accepting new connections 
 * 
 */
void Server::acceptConnections() {
    std::cout << "server accepting new connections..." << std::endl;
    handleMessages();
}

/**
 * @brief listens for new messages and checks their types, acting 
 * accordingly
 * 
 */
void Server::handleMessages() {
    Packet p;
    sockaddr_in *p_info;
    while(running) {
        p = rec_packet(sock, &p_info);
        
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
                sendPrivate(p);
                break;
            default:
                break;
        }
    }

}

/**
 * @brief sends a private message to a user
 * 
 * @param p 
 * @return int 
 */
int Server::sendPrivate(Packet p) {
    std::string to(p.getMessage().substr(0, p.getMessage().find_first_of(' ')));
    std::cout << to.length() << std::endl;
    std::cout << "looking for " << to << std::endl;
    showUsers();
    std::string msg(p.getMessage().substr(p.getMessage().find_first_of(' ')));
    if(!userExists(to)) {
        sendToOne(to, srvInfo, getUser(p.getFrom()), NO_USER);
    }else {
        sendToOne(msg, getUser(p.getFrom()), getUser(to));
    }
    return 1;
    
}

/**
 * @brief prints out a list of users in the server
 * 
 */
void Server::showUsers() {
    std::cout << "-- USERS --" << std::endl;
    for (std::map<std::string, ClientInfo>::iterator it = connections.begin();
    it != connections.end(); it++) {
        std::cout << "- " << it->first << std::endl;
    }
}

/**
 * @brief checks a potential user's name, adding them to the chatroom if it isn't
 * taken already and rejecting them if it is, prompting for a different one.
 * 
 * @param p 
 * @param p_info 
 */
void Server::welcomeUser(Packet p, sockaddr_in *p_info) {
    ClientInfo ci(p.getFrom(), p_info);
    
    if(userExists(p.getFrom())) {
        sendMessage("USERNAME ALREADY EXISTS", MessageType::USER_REJECT, srvInfo, ci);
        return;
    }

    connections.emplace(p.getFrom(), ci);
    std::string user_list = "";
    for(std::map<std::string, ClientInfo>::iterator user = connections.begin();
            user != connections.end(); user++) {
        user_list += user->first + ",";
    }
    
    showUsers();
    
    sendMessage(user_list, MessageType::USER_ACCEPT, srvInfo, ci);
    sendToAll(ci.getUsername(), ci, MessageType::NEW_USER);

    std::cout << "added user " << p.getFrom() << std::endl;

}

/**
 * @brief sends msg of type type from sender to receiver. 
 * 
 * @param msg 
 * @param type 
 * @param sender 
 * @param receiver 
 * @return int 
 */
int Server::sendMessage(std::string msg, MessageType type, ClientInfo sender, ClientInfo receiver) {
    Packet p(sender.getUsername(), msg, type, seq);
    
    if(send_packet(sock, *receiver.getInfo(), p)  == 0) {
        std::cout << "problem sending message to " << receiver.getUsername() << std::endl;
        return -1;
    }
    
    std::cout << "sent message " << p.getMessage() << " to " << receiver.getUsername() <<  std::endl;
    return 1;
}

/**
 * @brief removes a user from the chatroom, notifying all others that the user 
 * left.
 * 
 * @param username 
 */
void Server::removeUser(std::string username) {
    ClientInfo left = getUser(username);
    sendToAll("USER " + username + " LEFT", left, MessageType::USER_EXIT);
    sendToOne("Goodbye.", srvInfo, left, MessageType::USER_GOODBYE);
    connections.erase(username);
}

/**
 * @brief notifies all users that the server is closing. 
 * 
 */
void Server::closeRoom() {
    sendToAll("SERVER CLOSING", srvInfo, MessageType::SERVER_CLOSE);
}

/**
 * @brief sends message from sender to receiver
 * 
 * @param message 
 * @param sender 
 * @param receiver 
 * @param __type 
 * @return int 
 */
int Server::sendToOne(std::string message, ClientInfo sender, ClientInfo receiver, MessageType __type) {
    if(sendMessage(message, __type, sender, receiver) < 0) {
        std::cout << "problem sending message to user " << receiver.getUsername() << std::endl;
        return -1;
    }
    return 1;
}

/**
 * @brief sends message from sender to all users in room of type type. 
 * 
 * @param message 
 * @param sender 
 * @param type 
 * @return int 
 */
int Server::sendToAll(std::string message, ClientInfo sender, MessageType type) {
    for(std::map<std::string, ClientInfo>::iterator it = connections.begin();
            it != connections.end(); it++) {
        if(sender.getUsername() == it->second.getUsername()) continue;
        if(sendMessage(message, type, sender, it->second) < 0){
            std::cout << "problem sending message to user " << it->second.getUsername() << std::endl;
            return -1;
        }
    }
    return 1;
}

/**
 * @brief checks a user's membership in the chatroom
 * 
 * @param username 
 * @return true 
 * @return false 
 */
bool Server::userExists(std::string username) {
    return connections.count(username) == 1;
}

/**
 * @brief gets a specific user from the chatroom's member list
 * 
 * @param username 
 * @return ClientInfo 
 */
ClientInfo Server::getUser(std::string username) {
    return connections[username];
}

bool Server::getRunning() {
    return running;
}

/**
 * @brief Destroy the Server:: Server object
 * 
 */
Server::~Server() {
    for(std::map<std::string, ClientInfo>::iterator it = connections.begin();
    it != connections.end(); it++) {
        free(it->second.getInfo());
    }
}