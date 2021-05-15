#include "Packet.hpp"
#include <string> 

Packet::Packet(std::string From, std::string Message, MessageType Type, short Seq) {
    from = From;
    message = Message;
    type = Type;
    seq = Seq;
}

Packet::Packet() {}

Packet::~Packet() {}

std::string Packet::getFrom() { return from; }
std::string Packet::getMessage() { return message; }
MessageType Packet::getType() { return type; }
short Packet::getSeq() { return seq; }

void Packet::setType(MessageType Type) { type = Type; }
void Packet::setFrom(std::string From) { from = From; }
void Packet::setMessage(std::string Message) { message = Message; }
void Packet::setSeq(short Seq) { seq = Seq; }
        
void Packet::addToMessage(char x) {
    message += x;
}
void Packet::addToUsername(char x) {
    from += x;
}