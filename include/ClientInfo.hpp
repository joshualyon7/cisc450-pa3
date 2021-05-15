/**
 * @file ClientInfo.hpp
 * @author Josh Lyon (joshlyon@udel.edu)
 * @brief class to hold client info
 * @date 2021-05-14
 * 
 * 
 */
#pragma once
#include <iostream>
#include <sys/types.h>
#include <sys/types.h>
#include <netinet/in.h>
class ClientInfo {
    private:
        sockaddr_in *addr;
        std::string username;

    public:
        ClientInfo(std::string username, sockaddr_in *addr);
        ClientInfo();
        std::string getUsername();
        sockaddr_in *getInfo();
        void setUsername(std::string Username);
        void setAddr(sockaddr_in *Addr);
        ~ClientInfo();
};

