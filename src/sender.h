#ifndef SENDER_H
#define SENDER_H

#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

//#define RECEIVER_ADDR "172.19.13.191"
#define RECEIVER_PORT 44445

int send_msg( char *ip, char *data );

#endif