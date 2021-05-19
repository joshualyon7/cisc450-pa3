/**
 * @file ClientInfo.cpp
 * @author josh lyon (joshlyon@udel.edu)
 * @brief contains implementation of functions from the ClientInfo class
 * @version 0.1
 * @date 2021-05-19
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <iostream>
#include <ClientInfo.hpp>

/**
 * @brief Construct a new Client Info:: Client Info object
 * 
 * @param Username 
 * @param Addr 
 */
ClientInfo::ClientInfo(std::string Username, sockaddr_in *Addr) {
    username = Username;
    addr = Addr;
    addr->sin_family = AF_INET;
}

ClientInfo::ClientInfo() {};

std::string ClientInfo::getUsername() { return username; }
sockaddr_in *ClientInfo::getInfo() { return addr; }
void ClientInfo::setUsername(std::string Username) { username = Username; }
void ClientInfo::setAddr(sockaddr_in *Addr) { addr = Addr; }

ClientInfo::~ClientInfo() {}
