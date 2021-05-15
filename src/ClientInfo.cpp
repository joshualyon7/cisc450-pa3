#include <iostream>
#include <ClientInfo.hpp>

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
