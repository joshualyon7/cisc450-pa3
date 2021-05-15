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
 * @brief serializes a data packet (found at https://stackoverflow.com/a/20838004)
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
    std::cout << data << std::endl;
}

/**
 *  @brief deserializes a data packet (found at https://stackoverflow.com/a/20838004)
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
 * @brief send a packet containing data on sd
 * 
 * @param sd socket descriptor to send from
 * @param data string of data to send
 * @param seq sequence number to send the data packet with
 * @param rec_info info of the receiver to send to 
 * @return number of bytes sent on success, -1 on failure
 */
int send_packet(int sd, sockaddr_in receiver_info, Packet packet) {
    int bytes_sent, count = packet.getMessage().length();

    if(packet.getMessage().length() > MAX_LINE) {
        std::cout << "message too long" << std::endl;
        return -1;
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
    
    
    if(count) std::cout << "Packet " << packet.getSeq() << " succesfully transmitted with " << (HEADER_SIZE + count + packet.getFrom().length()) << " data bytes" << std::endl;
    else std::cout << "End of Transmission Packet with sequence number " << packet.getSeq() << " transmitted" << std::endl;

    return bytes_sent;
}




/**
 * @brief receives a packet and stores its data in 'data'
 * 
 * @param sd socket descriptor to receive from
 * @param data pointer to string to store the packet's data
 * @param seq_no pointer to int to store the sequence number
 * @param send_info info of the sender to receive from
 * @return number of bytes received on success, -1 on failure
 */
Packet rec_packet(int sd, sockaddr_in *sender_info) {
    int bytes_received;
    char buff[BUFFER_MAX];
    DataPacket rawPacket;
    
    socklen_t sender_len = sizeof(*sender_info);
    memset(sender_info, 0, sizeof(sender_info));

    
    if((bytes_received = recvfrom(sd, buff, BUFFER_MAX, 0, 
            (struct sockaddr *)&sender_info, &sender_len)) < 0) {
        perror("recv failed with");
        Packet errPkt;
        return errPkt;
    }
    
    Packet recPkt;
    recPkt = deserialize(buff);
    std::cout << "after memset" << std::endl;
    std::cout << "rec IP: " << inet_ntoa(sender_info->sin_addr) << std::endl;
    
    return recPkt;
}

