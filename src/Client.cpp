#include "Client.hpp"
#include "pa.hpp"
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include<bits/stdc++.h>
#include <unistd.h>

Client::Client() {
    mtx = new std::mutex();

}
Client::~Client() {
    delete mtx;
}

int Client::joinServer() {
    Packet p;
    MessageType status = MessageType::USER_REJECT;

    while (status == USER_REJECT) {
        mtx->lock();
        display.writeOut("Welcome! Please choose a username to get started.");
        username = display.getInput();
        mtx->unlock();
        if(sendMessage(username, MessageType::NEW_USER) < 0) {
            display.writeOut("error sending username to server");
            return -1;
        }
        p = receiveMessageRaw();
        if ((status = p.getType()) == MessageType::USER_REJECT) {
            mtx->lock();
            display.writeOut("Username taken.");
            mtx->unlock();
        }
    }
    
    populateUserList(p.getMessage());
    printUsers();
    listen();

    return 0;
}


int Client::connectToServer(std::string ip, int port, sa_family_t family, int connType) {
    mtx->lock();
    display.writeOut("connecting to server...");
    mtx->unlock();
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

    mtx->lock();
    display.writeOut("client connected to server!");
    mtx->unlock();
    
    return 1;
}

int Client::sendMessage(std::string msg, MessageType type) {
    Packet p(username, msg, type, seq);

    if(send_packet(sock, server, p) < 0) {
        display.writeOut("problem sending packet...");
        return -1;
    }
    return 1;
}

int Client::sendPrivate(std::string input) {
    input = input.substr(input.find_first_of(' '));

    if(sendMessage(input, MessageType::PRIVATE_CHAT) < 0) {
        display.writeOut("problem sending message...");
        return -1;
    }

    return 1;
}

int Client::sendPublic(std::string msg) {
    //std::cout << "trying to send " << msg << std::endl;
    if(sendMessage(msg, MessageType::PUBLIC_CHAT) < 0) {
        display.writeOut("problem sending message...");
        return -1;
    }
    //std::cout << "after sending " << msg << std::endl;

    return 1;
}

Packet Client::receiveMessageRaw() {
    sockaddr_in *srvPtr = &server;
    return rec_packet(sock, &srvPtr);
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
    mtx->lock();
    display.writeOut("-- USERS --");
    
    for(std::vector<std::string>::iterator user = users.begin(); user != users.end(); user++) {
        display.writeOut("> " + *user);
    }
    mtx->unlock();
}

void Client::printHelp() {
    mtx->lock();
    std::string cmdStrings[] = {
        "/private [user] - sends a private message to a user",
        "/list - lists all users in the chatroom",
        "/leave - leaves the chatroom",
        "/help - prints this help string"
    };
    display.writeOut("\n-- HELP --");
    display.writeOut("Commands (/[command])");
    for(std::string cmd : cmdStrings) {
        display.writeOut("- " + cmd);
    }
    mtx->unlock();
}

void Client::listen() {
    listener = std::thread(&Client::listenToServer, this);
    mtx->lock();
    display.writeOut("listener thread started listening to server");
    mtx->unlock();

}

void Client::stopListening() {
    running = false;
    listener.join();
    display.writeOut("stopped listening for messages from server");
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
                mtx->lock();
                display.writeOut("Server closed unexpectedly");
                mtx->unlock();
                leaveServer();
                return;
            default:
                break;
        }
    }
    
}

void Client::displayMessage(std::string message, std::string sender, bool priv) {
    display.writeOut("before lock()");
    sleep(5);
    mtx->lock();
    display.writeOut("after lock()");
    display.writeOut(sender + (priv ? "(Private): " : ": ") + message);
    mtx->unlock();
}

void Client::addUser(std::string user) {
    std::string pre("Server: User ");
    std::string suffix(" has entered the chatroom!");
    users.push_back(user);
    display.writeOut("before lock");
    mtx->lock();
    display.writeOut(pre.append(user).append(suffix));
    mtx->unlock();
}

void Client::removeUser(std::string user) {
    std::remove(users.begin(), users.end(), user);
    mtx->lock();
    display.writeOut("User " + user + " has left the chatroom!");
    mtx->unlock();
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
Display Client::getDisplay() {
    return display;
}