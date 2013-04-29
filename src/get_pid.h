#ifndef GET_PID_H
#define GET_PID_H

#define PROC "/proc"
#define CMDLINE "/cmdline"

#define MAX_DIR_SIZE 256
#define MAX_CONTENT_SIZE 1024

int get_pid( char *proc_name );

#endif
