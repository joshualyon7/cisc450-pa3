/*
 * @file netutils.c
 * @author Josh Lyon (joshlyon@udel.edu)
 * @brief Contains utilities for sending and receiving packets with UDP 
 * 
 */

#include "pa.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include "config.hpp"
#include "Packet.hpp"

/**
 * @brief serializes a data packet (adapted from https://stackoverflow.com/a/20838004)
 * 
 * @param packet 
 * @param data 
 */
void serialize(DataPacket *packet, char *data)
{
    int *q = (int*)data;    
    *q = packet->count;     q++;
    *q = packet->unamelen;     q++;
    *q = packet->type;       q++; 
    *q = packet->seq_no;   q++; 

    char *p = (char*)q;
    int i = 0;
    while (i < packet->count)
    {
        *p = packet->message[i];
        p++;
        i++;
    }
    
    i = 0;
    while (i < packet->unamelen)
    {
        *p = packet->username[i];
        p++;
        i++;
    }
}

/**
 *  @brief deserializes a data packet (adapted from https://stackoverflow.com/a/20838004)
 *  
 *  @param packet 
 *  @param data 
 */
Packet deserialize(char *data)
{
    Packet retPkt;
    std::string message, uname;
    message.resize(MAX_LINE);
    uname.resize(MAX_UNAME);
    retPkt.setMessage("");
    retPkt.setFrom("");
    int *q = (int*)data;    
    int count = *q; q++;
    int ulen = *q; q++;
    retPkt.setType((MessageType)*q);  q++;    
    retPkt.setSeq(*q); q++;    


    char *p = (char*)q;
    int i = 0;
    while (i < count)
    {
        //retPkt.getMessage()[i] = *p;
        message[i] = *p;
        p++;
        i++;
    }
    i = 0;
    while (i < ulen)
    {
        uname[i] = *p;
        p++;
        i++;
    }
    retPkt.setMessage(message);
    retPkt.setFrom(uname);
    
    return retPkt;
}

/**
 * @brief sends packet to receiver on sd
 * 
 * @param sd 
 * @param receiver_info 
 * @param packet 
 * @return int 
 */
int send_packet(int sd, sockaddr_in receiver_info, Packet packet) {
    int bytes_sent, count = packet.getMessage().length();

    if(packet.getMessage().size() > MAX_LINE) {
        std::cout << packet.getMessage().length() << " " << packet.getMessage() << std::endl;
        return 0;
    }

    DataPacket toSerialize = {
        count,
        (int)packet.getFrom().length(),
        packet.getType(),
        packet.getSeq(),
        packet.getMessage(),
        packet.getFrom()
    };

    char buff[BUFFER_MAX];
    
    serialize(&toSerialize, buff);
    
    socklen_t rec_addr_len = sizeof(receiver_info);
    
    if((bytes_sent = sendto(sd, &buff, BUFFER_MAX, 0, 
       (struct sockaddr*)&receiver_info, rec_addr_len)) < 0) {
        perror("sending failed");
        return -1;
    
    }
    
    return bytes_sent;
}

/**
 * @brief receives a packet (which it returns) and stores the sender's info in sender_info
 * 
 * @param sd 
 * @param sender_info 
 * @return Packet 
 */
Packet rec_packet(int sd, sockaddr_in **sender_info) {
    int bytes_received;
    char buff[BUFFER_MAX];
    DataPacket rawPacket;
    
    sockaddr_in *sender_in = (sockaddr_in*)malloc(sizeof(sockaddr_in));
    socklen_t sender_len = sizeof(*sender_in);
    memset(sender_in, 0, sizeof(sender_len));
    
    if((bytes_received = recvfrom(sd, buff, BUFFER_MAX, 0, 
            (struct sockaddr *)sender_in, &sender_len)) < 0) {
        perror("recv failed with");
        Packet errPkt;
        return errPkt;
    }
    
    *sender_info = sender_in;
    
    Packet recPkt;
    recPkt = deserialize(buff);
    
    return recPkt;
}

