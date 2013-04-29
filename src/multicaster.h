#ifndef MULTICASTER_H
#define MULTICASTER_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<time.h>
 
#define HELP_PORT 44444
#define HELP_GROUP "225.0.0.37"

int multicast();

#endif