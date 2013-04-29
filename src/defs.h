#ifndef DEFS_H
#define DEFS_H

//used by watchman.c
#define MY_ADDR "172.0.0.1" 
#define MY_PORT 44445
//setting MAX_ALLOWED_CLIENTS To its max limit and testing
#define MAX_ALLOWED_CLIENTS 127
#define MAX_SENDER_MSG_SIZE 1024
#define MAX_DIR_SIZE 256
#define MAX_DIR_NAME_SIZE 64
#define DIR_PERM 0777
#define FILE_PERM 0777


#define VOLUNTEER_TXT "volunteer.txt"
#define VOLUNTEER_TXT_MODE "a"
#define VOLUNTEER_TXT_PATH "/tmp/volunteer.txt"

#define NEEDY_TXT "needy.txt"
#define NEEDY_TXT_PATH "/tmp/needy.txt"

#define BASE_NEEDY "/tmp/needy"
#define BASE_VOL "/tmp/vol"
#define BASH_SHELL "/bin/bash"
#define SH_SHELL "/bin/sh" 

//for multicast_watchman
#define HELP_PORT 44444
#define HELP_GROUP "225.0.0.37"
#define MSGBUFSIZE 9
#define NEEDY_TXT_MODE "a"
#define POLICY_TXT "policy.txt"
#define POLICY_TXT_MODE "r"
#define NEEDY_DENY_LIST "needy.deny"
#define MAX_DENY_LIST_SIZE 30

//for distapp.c
#define VOL_DENY_LIST "vol.deny"

#define SHMSIZE 16
#define SHMKEY 1234

#endif