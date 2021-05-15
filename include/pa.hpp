#include "Packet.hpp"
#include <string>
#include "types.hpp"
#include <netinet/in.h> 
#pragma once

int send_packet(int sd, sockaddr_in receiver_info, Packet packet);
Packet rec_packet(int sd, sockaddr_in **sender_info);
Command strToCommand(std::string cmd);