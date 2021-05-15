#pragma once
#include <string>
#include "types.hpp"
class Packet {
    private:
        std::string from;
        std::string message;
        MessageType type;
        short seq;
    public:
        Packet(); 

        Packet(std::string from, std::string message, MessageType type, short seq);

        std::string getFrom();
        std::string getMessage();
        MessageType getType();
        short getSeq();
        
        void setType(MessageType Type);
        void setFrom(std::string From);
        void setMessage(std::string Message);
        void setSeq(short Seq);
        
        void addToMessage(char x);
        void addToUsername(char x);
        
        ~Packet();
};

