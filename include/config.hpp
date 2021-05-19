/**
 * @file config.h
 * @author Josh Lyon (joshlyon@udel.edu)
 * @brief contains config values for UDP chatroom program
 * 
 */

// port for the server to listen on
#define SERVER_PORT 18000
// server's address
#define SERVER_ADDR "127.0.0.1"
// maximum allowable line to be transmitted
#define MAX_LINE 80
// maximum allowable length for a username
#define MAX_UNAME 30
// buffer size to send and receive with
#define BUFFER_MAX 512
// length of a header 
#define HEADER_SIZE sizeof(int) + sizeof(int) + sizeof(int) + sizeof(short)