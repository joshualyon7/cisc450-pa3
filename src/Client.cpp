#include "Client.hpp"
#include "pa.hpp"
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include<bits/stdc++.h>

Client::Client() {}
Client::~Client() {}

int Client::joinServer() {
    Packet p;
    MessageType status = MessageType::USER_REJECT;

    while (status == USER_REJECT) {
        std::cout << "Choose a username: ";
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

int Client::sendMessage(std::string msg, MessageType type) {
    Packet p(username, msg, type, seq);

    if(send_packet(sock, server, p) < 0) {
        std::cout << "problem sending packet" << std::endl;
        return -1;
    }
    return 1;
}

int Client::sendPrivate(std::string input) {
    input = input.substr(input.find_first_of(' '));

    if(sendMessage(input, MessageType::PRIVATE_CHAT) < 0) {
        std::cout << "problem sending message" << std::endl;
        return -1;
    }

    return 1;
}

int Client::sendPublic(std::string msg) {
    if(sendMessage(msg, MessageType::PUBLIC_CHAT) < 0) {
        std::cout << "problem sending message" << std::endl;
        return -1;
    }

    return 1;
}

Packet Client::receiveMessageRaw() {
    return rec_packet(sock, &server);
}

void Client::populateUserList(std::string users_raw) {
    std::string user = "";
    for (char x : users_raw) {
        if (x == ',') {
            users.push_back(user);
            user = "";
        } else {
            user += x;
        }
    }
}

void Client::printUsers() {
    std::cout << "-- USERS --" << std::endl;
    
    for(std::vector<std::string>::iterator user = users.begin(); user != users.end(); user++) {
        std::cout << "> " << *user << std::endl;
    }
}

void Client::printHelp() {
    std::string cmdStrings[] = {
        "/private [user] - sends a private message to a user",
        "/list - lists all users in the chatroom",
        "/leave - leaves the chatroom",
        "/help - prints this help string"
    };
    std::cout << "-- HELP --" << std::endl;
    std::cout << "Commands (/[command])" << std::endl;
    for(std::string cmd : cmdStrings) {
        std::cout << "- " << cmd << std::endl;
    }
}

void Client::listen() {
    listener = std::thread(&Client::listenToServer, this);
    std::cout << "listener thread started listening to server" << std::endl;

}

void Client::stopListening() {
    running = false;
    listener.join();
    std::cout << "stopped listening for messages from server" << std::endl;
}

void Client::listenToServer() {
    Packet p;
    while(running) {
        p = receiveMessageRaw();
        switch(p.getType()) {
            case MessageType::PUBLIC_CHAT:
                displayMessage(p.getMessage(), p.getFrom(), false);
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
            default:
                break;
        }
    }
    
}

void Client::displayMessage(std::string message, std::string sender, bool priv) {
    std::cout << sender << (priv ? "(Private): " : ": ") << message << std::endl;
}

void Client::addUser(std::string user) {
    users.push_back(user);
    std::cout << "User " << user << " has entered the chatroom!" << std::endl;
}

void Client::removeUser(std::string user) {
    std::remove(users.begin(), users.end(), user);
    std::cout << "User " << user << " has left the chatroom." << std::endl;
}

int Client::leaveServer() {
    stopListening();
    sendMessage("", MessageType::USER_EXIT);
    return 1;
}

// mutators
std::string Client::getUsername() {
    return username;
}
std::vector<std::string> Client::getUsers() {
    return users;
}
sockaddr_in Client::getServer() {
    return server;
}
int Client::getSock() {
    return sock;
}