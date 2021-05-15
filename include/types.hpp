#pragma once
/**
 * @file pa1.h
 * @author Josh Lyon (joshlyon@udel.edu)
 * @brief contains type definitions and shared includes/function declarations
 * for file sharing over TCP. 
 * 
 */
#include <string>

#define EOT ""

enum MessageType {
    NEW_USER,
    USER_EXIT,
    USER_ACCEPT,
    USER_REJECT,
    PUBLIC_CHAT,
    PRIVATE_CHAT, 
    NO_USER,
    SERVER_CLOSE
};


enum Command {
    PRIVATE,
    LIST,
    LEAVE, 
    HELP
};

struct DataPacket {
    int count;
    int unamelen;
    MessageType type;
    short seq_no;
    std::string message;
    std::string username;
};

// enum for packet-printing options
typedef enum P_OPTION{
    SEND_PACKET,
    REC_PACKET,
    SEND_EOT,
    REC_EOT, 
    SEND_ACK, 
    REC_ACK, 
    DROP_PACKET, 
    DROP_ACK
} P_OPTION_t;
