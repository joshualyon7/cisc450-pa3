/**
 * @file Client.cpp
 * @author josh lyon (joshlyon@udel.edu)
 * @brief contains logic for a client object's functions
 * @version 0.1
 * @date 2021-05-19
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "Client.hpp"
#include "config.hpp"
#include "pa.hpp"
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include<bits/stdc++.h>
#include <unistd.h>
#include <cstring>

Client::Client() {
}
Client::~Client() {
}

/**
 * @brief prompts the user for a username and listens for a reply from the server,
 * prompting again if the username is invalid
 * 
 * @return int 
 */
int Client::joinServer() {
    Packet p;
    MessageType status = MessageType::USER_REJECT;

    std::cout << "Welcome! Please choose a username to get started." << std::endl;
    while (status == USER_REJECT) {
        std::cin >> username;
        if(sendMessage(username, MessageType::NEW_USER) < 0) {
            std::cout << "error sending username to server" << std::endl;
            return -1;
        }
        p = receiveMessageRaw();
        if ((status = p.getType()) == MessageType::USER_REJECT) {
            std::cout << "Username taken." << std::endl;
        }
    }
    
    populateUserList(p.getMessage());
    printUsers();
    listen();

    return 0;
}

/**
 * @brief sets up the listening socket and tries to connect to the server
 * 
 * @param ip 
 * @param port 
 * @param family 
 * @param connType 
 * @return int 
 */
int Client::connectToServer(std::string ip, int port, sa_family_t family, int connType) {
    std::cout << "connecting to server..." << std::endl;
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_port = htons(port); 
    server.sin_family = family;
    
    if((sock = socket(family, connType, 0)) < 0) {
        perror("creating socket failed with");
        return -1;
    }
    
    if(connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        perror("connecting to server failed with");
        return -1;
    }

    std::cout << "client connected to server!" << std::endl;
    
    return 1;
}

/**
 * @brief sends a message of type type to the server.
 * 
 * @param msg 
 * @param type 
 * @return int 
 */
int Client::sendMessage(std::string msg, MessageType type) {
    Packet p(username, msg, type, seq);

    if(send_packet(sock, server, p) < 0) {
        std::cout << "problem sending packet..." << std::endl;
        return -1;
    }
    return 1;
}

/**
 * @brief sends a private message to the user denoted by the first word of input. 
 * 
 * @param input 
 * @return int 
 */
int Client::sendPrivate(std::string input) {
    if(input.find_first_of(' ') == std::string::npos) {
        std::cout << "Please enter a user." << std::endl;
        return -1;
    }
    input = input.substr(input.find_first_of(' ') + 1);
    if(sendMessage(input, MessageType::PRIVATE_CHAT) < 0) {
        std::cout << "problem sending message..." << std::endl;
        return -1;
    }

    return 1;
}

/**
 * @brief sends a public message to the server.
 * 
 * @param msg 
 * @return int 
 */
int Client::sendPublic(std::string msg) {
    if(sendMessage(msg, MessageType::PUBLIC_CHAT) < 0) {
        std::cout << "problem sending message..." << std::endl;
        return -1;
    }

    return 1;
}

/**
 * @brief receives a raw, unparsed packet from the server
 * 
 * @return Packet 
 */
Packet Client::receiveMessageRaw() {
    sockaddr_in *srvPtr = &server;
    Packet p = rec_packet(sock, &srvPtr);
    free(srvPtr);
    return p;
}

/**
 * @brief consumes a comma separated list of users and populates the Client's 
 * user list.
 * 
 * @param users_raw 
 */
void Client::populateUserList(std::string users_raw) {
    
    std::string user;
    for (char x : users_raw) {
        if (x == ',') {
            users.push_back(user);
            std::cout << "added " << user << " while popoulating l;ist" << std::endl;
            user = "";
        } else {
            user += x;
        }
    }
    if (user[0]) {
        users.push_back(user);
    } 
}

/**
 * @brief lists all users the Client knows are in the room
 * 
 */
void Client::printUsers() {
    std::cout << "\n-- USERS --" << std::endl;
    
    for(std::list<std::string>::iterator user = users.begin(); user != users.end(); user++) {
        std::cout << "> " << *user << std::endl;
    }
    std::cout << std::endl;
}

/**
 * @brief prints out the help string, containing all of the commands
 * that can be run on the server
 * 
 */
void Client::printHelp() {
    std::string cmdStrings[] = {
        "/private [user] - sends a private message to a user",
        "/list - lists all users in the chatroom",
        "/leave - leaves the chatroom",
        "/help - prints this help string"
    };
    std::cout << "\n-- HELP --" << std::endl;
    std::cout << "Commands (/[command])" << std::endl;
    for(std::string cmd : cmdStrings) {
        std::cout << "- " << cmd << std::endl;
    }
    std::cout << std::endl;
}

/**
 * @brief spawns a new thread to listen to the server for incoming messages
 * 
 */
void Client::listen() {
    listener = std::thread(&Client::listenToServer, this);
    std::cout << "listener thread started listening to server..." << std::endl;

}

/**
 * @brief cleans up outstanding threads 
 * 
 */
void Client::stopListening() {
    running = false;
    listener.join();
    std::cout << "stopped listening for messages from server..." << std::endl;
}

/**
 * @brief listens for messages from the server and acts accordingly based on 
 * their type
 * 
 */
void Client::listenToServer() {
    Packet p;
    while(running) {
        p = receiveMessageRaw();
        switch(p.getType()) {
            case MessageType::PUBLIC_CHAT:
                displayMessage(p.getMessage(), p.getFrom(), false);
                break;
            case MessageType::NO_USER:
                std::cout << "User " << p.getMessage() << " not found." << std::endl;
                break;
            case MessageType::PRIVATE_CHAT:
                displayMessage(p.getMessage(), p.getFrom(), true);
                break;
            case MessageType::NEW_USER:
                addUser(p.getMessage());
                break;
            case MessageType::USER_EXIT:
                removeUser(p.getFrom());
                break;
            case MessageType::SERVER_CLOSE:
                std::cout << "Server closed unexpectedly" << std::endl;
                leaveServer();
                return;
            case MessageType::USER_GOODBYE:
                running = false;
                break;
            default:
                break;
        }
    }
    
}

/**
 * @brief prints out message from sender to the screen
 * 
 * @param message 
 * @param sender 
 * @param priv 
 */
void Client::displayMessage(std::string message, std::string sender, bool priv) {
    std::cout << (sender + (priv ? "(Private): " : ": ") + message) << std::endl;
}

/**
 * @brief adds a user to the Client's user list and prints a message to the 
 * user's screen
 * 
 * @param user 
 */
void Client::addUser(std::string user) {
    std::string pre("Server: User ");
    std::string suffix(" has entered the chatroom!");
    if(!std::binary_search(users.begin(), users.end(), user)){
        users.push_back(user);
        std::cout << "added " << user << std::endl;
    }
    std::cout << pre << user << suffix << std::endl;
}

/**
 * @brief removes a user from the Client's user list and prints a message to 
 * the screen
 * 
 * @param user 
 */
void Client::removeUser(std::string user) {
    users.remove(user);
    users.sort();
    std::cout << "User " << user << " has left the chatroom!" << std::endl;
}

/**
 * @brief notifies the server that the Client is disconnecting from the server.
 * 
 * @return int 
 */
int Client::leaveServer() {
    sendMessage("", MessageType::USER_EXIT);
    stopListening();
    return 1;
}

// mutators
std::string Client::getUsername() {
    return username;
}
std::list<std::string> Client::getUsers() {
    return users;
}
sockaddr_in Client::getServer() {
    return server;
}
int Client::getSock() {
    return sock;
}
