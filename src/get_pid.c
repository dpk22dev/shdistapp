#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<dirent.h>
#include<errno.h>
#include<sys/types.h>

#include "get_pid.h"

int get_pid( char *proc_name ){


	int pid = -1;
	int id;
	DIR *dp = opendir( PROC );
	char filename[MAX_DIR_SIZE];
	char content[MAX_CONTENT_SIZE];
	
	FILE *ifp;
	
	if( dp != NULL ){

		struct dirent* dirp;
		while( dirp = readdir(dp) ){
			id = atoi( dirp -> d_name );
			//printf("file:%s id: %d\n", dirp -> d_name, id );
			//dir only for pid
			if( id > 0 ){
				sprintf( filename, PROC "/%d" CMDLINE , id );
				//printf("checking file: %s\n", filename );
				ifp = fopen( filename, "r" );
				if( ifp != NULL ){
					fscanf( ifp,"%s", content );
					if( strstr( content, proc_name ) != NULL ){
						//found the process
						pid = id;
						break;
					}		
					fclose( ifp );
				}else{
					fprintf(stderr,"%s[%d]: Error %d in reading file %s>%s\n",__FILE__,__LINE__, errno, filename, strerror(errno) );	
				}
				
			}
		}		
	}else{
		fprintf(stderr,"%s[%d]: Error %d in reading /proc dir>%s\n",__FILE__,__LINE__, errno, strerror(errno) );
	}

	if( ifp )
		fclose ( ifp );
	if( dp )
		closedir( dp );
	return pid;
	
}

/*
int main( ){

	int pid;
	char *proc_name = "getpid";
	pid = get_pid( proc_name );
	printf("%s pid: %d\n",proc_name, pid );
	while(1);
}
*/
