#include "watchman.h"
#include "headers.h"
#include "dir_sender.h"
#include "ip.h"
#include "dir_op.h"
#include "scripts.h"
#include "defs.h"
#include "get_pid.h"

#include <glib.h>
#include "hashtab.h"

//be careful with global defined variables as anyone can change them

GHashTable *ipfd = NULL, *ipcwd = NULL;

//thread handler for populating volunteer list
void *populate_volunteer_list( void *ip_addr ){

	//****************** -----------> insert data in volunteer tab <---------------- ********************* 
	char *ip = ( char * )ip_addr;
	
	FILE *ofp = fopen(VOLUNTEER_TXT_PATH, VOLUNTEER_TXT_MODE);
	if( ofp != NULL ){
		fprintf( ofp,"%s\n", ip);
		fclose( ofp );			
	}else{
		fprintf(stderr,"Could not open "VOLUNTEER_TXT_PATH);		
	}

	return ;
}

void show_entries( ){

	printf("printing <ip,fd> entries\n");
	ipfd_show_all_entries( ipfd );
	printf("printing <ip,cwd> entries\n");
	ipcwd_show_all_entries( ipcwd );
	
}

int receive_msg( int my_fd ){

	char cwd[MAX_DIR_SIZE];
	char temp_arr[MAX_DIR_SIZE];	
	mode_t process_mask;	
	int sender_fd;
	//int *ptr_sender_fd = (int *)malloc( sizeof(int) );
	struct sockaddr_in sender_addr;

	int size_sockaddr_in = sizeof(struct sockaddr_in);
	
	//accept incoming connection
	sender_fd = accept( my_fd, (struct sockaddr*)&sender_addr, &size_sockaddr_in );
	
	if( sender_fd < 0 ){
		fprintf(stderr,"Could not accept\n");
		return -3;
	}
	//printf("connection accepted!\n");
	//get the ip of msg sender
	char *vol_ip = malloc( strlen( inet_ntoa(sender_addr.sin_addr) ) + 1 );	
	strcpy( vol_ip, inet_ntoa(sender_addr.sin_addr) );
	
	char sender_msg[MAX_SENDER_MSG_SIZE];
	size_t read_size;

	int fd = -1;
	
	while( ( read_size = recv( sender_fd, sender_msg, MAX_SENDER_MSG_SIZE, 0) ) > 0 ){
		
		sender_msg[read_size] = '\0';
//		printf( "msg: %s len: %d bytes: %d\n", sender_msg, strlen(sender_msg), read_size );
//		fprintf( stdout, sender_msg );
//		 write to opened file

		if( does_ip_exist(ipfd,vol_ip) == TRUE )
				fd = find_fd( ipfd, vol_ip ); 
		
		if( strcmp(sender_msg,"__FILE_CLOSE__") == 0 ){
			printf("received order: __FILE_CLOSE__ from: %s\n", vol_ip );
			//========== remove entry from ipfd ==============			
				if( fd > 0 ){
					close( fd );
					ipfd_rem_pair( ipfd, vol_ip );
				}			
		}
		else if( fd > 0){
			if( write( fd, sender_msg, read_size ) != read_size ){
				fprintf(stderr,"%s[%d]: Error %d in writing to file >%s\n",__FILE__,__LINE__, errno, strerror( errno ) );				
			}
		}
	
	}
	
	if( read_size == -1 ){		
		fprintf(stderr,"%s[%d]: Error %d in reading from socket>%s\n",__FILE__,__LINE__, errno, strerror(errno) );			
		return -4;
	}else{
		//used by sender side
		if( strcmp(sender_msg,"__READY_TO_HELP__") == 0 ){
			printf("received order: __READY_TO_HELP__ from: %s\n", vol_ip);
			//create a thread that will put ip in volunteer file
			//WRITE TO FILE
			if( does_ip_exist(ipcwd, vol_ip) == FALSE ){
				ipcwd_insert_pair( ipcwd, vol_ip, "GARBAGE");
				pthread_t writer;
						
				if( pthread_create( &writer, NULL, populate_volunteer_list, (void*)vol_ip ) < 0 ){
					fprintf(stderr,"%s[%d]: Error %d in creating thread populate_volunteer_list >%s\n",__FILE__,__LINE__,errno, strerror(errno) );
					perror(NULL);
				}
				pthread_join( writer, NULL );	
			}else{
				//volunteer is already being helped by me
				//so we will not put him in volunteer list
			}			

		}
		else if( strcmp( sender_msg, "__PREV_ENTRY_FOUND_IPCWD__" ) == 0 ){
			printf("received order: __PREV_ENTRY_FOUND_IPCWD__ from: %s\n", vol_ip );
			printf("Either you are already helping / being helped by same machine\n");
			/*stop sending further messages
			fd = find_fd( ipfd, vol_ip );
			if( fd > 0 )
				close( fd );
			ipfd_rem_pair( ipfd, vol_ip );
			ipcwd_rem_pair( ipcwd, vol_ip );
			*/
		}
		else if( strcmp( sender_msg, "__PREV_ENTRY_FOUND_IPFD__" ) == 0 ){
			printf("received order: __PREV_ENTRY_FOUND_IPFD__ from: %s\n", vol_ip);
			printf("Either you are already helping / being helped by same machine\n");			
			/*stop sending further messages
			fd = find_fd( ipfd, vol_ip );
			if( fd > 0 )
				close( fd );
			ipfd_rem_pair( ipfd, vol_ip );
			ipcwd_rem_pair( ipcwd, vol_ip );
			*/
		}
		else if( strcmp( sender_msg, "__NO_SCRIPTS_DIR_FOUND__" ) == 0 ){
			printf("received order: __NO_SCRIPTS_DIR_FOUND__ from: %s\n", vol_ip);
			printf("No scripts dir found in task\n");
			/*stop sending further messages
			fd = find_fd( ipfd, vol_ip );
			if( fd > 0 )
				close( fd );
			ipfd_rem_pair( ipfd, vol_ip );
			ipcwd_rem_pair( ipcwd, vol_ip );
			*/
		}
		//used by receiver side
		//initialise the dir creation /tmp/needy/ip_of_needy
		else if( strcmp( sender_msg, "__INIT_OVERWRITE_DIR_NEEDY__") == 0 ){						
			printf("received order: __INIT_OVERWRITE_DIR_NEEDY__ from: %s\n", vol_ip);
			if( does_ip_exist( ipcwd, vol_ip ) == TRUE ){
				if( strcmp( find_cwd(ipcwd, vol_ip), "GARBAGE" ) != 0 )
					send_msg( vol_ip, "__PREV_ENTRY_FOUND_IPCWD__");				
			}
			
			sprintf( temp_arr, BASE_NEEDY"/%s", vol_ip );
			if( is_dir_exist( temp_arr ) == 1 ){
				if( chdir( temp_arr ) != 0 ){
					fprintf(stderr,"%s[%d]: Error %d in changing dir to %s >%s\n",__FILE__,__LINE__,errno,temp_arr, strerror(errno) );					
				}
			}else if( is_dir_exist( temp_arr ) == 0 ){ 
				//if dir doesn't exist
				if( init_path( vol_ip, BASE_NEEDY ) < 0 ) //path is not initialised
					return -1; //else path is initialised
		
				if( chdir( temp_arr ) != 0 ){
					fprintf(stderr,"%s[%d]: Error %d in changing dir to %s >%s\n",__FILE__,__LINE__,errno,temp_arr, strerror(errno) );
				}
			}
			//check using cwd and create own string by guess or append and compare and then show message
			//========== get cwd and put in ipcwd ==============
			if( getcwd(cwd, sizeof(cwd)) == NULL ){
				fprintf(stderr,"%s[%d]: Error %d in getting cwd %s\n",__FILE__,__LINE__, errno, strerror(errno) );
			}
			ipcwd_insert_pair( ipcwd, vol_ip, cwd );
			
		}
		//delete the previous dir and start fresh
		else if( strcmp( sender_msg, "__INIT_DELETE_DIR_NEEDY__") == 0 ){			
			printf("received order: __INIT_DELETE_DIR_NEEDY__ from: %s\n", vol_ip);
			if( does_ip_exist( ipcwd, vol_ip ) == TRUE ){
				if( strcmp( find_cwd(ipcwd, vol_ip), "GARBAGE" ) != 0 )
					send_msg( vol_ip, "__PREV_ENTRY_FOUND_IPCWD__");				
			}
			
			sprintf( temp_arr, BASE_NEEDY"/%s", vol_ip );
			if( is_dir_exist( temp_arr ) == 1 ){
				//remove dir below /tmp/needy/vol_ip <---also get removed
				remove_dir( temp_arr );
				init_path( vol_ip, BASE_NEEDY );

				if( chdir( temp_arr ) != 0 ){
					fprintf(stderr,"%s[%d]: Error %d in changing dir to %s >%s\n",__FILE__,__LINE__,errno,temp_arr, strerror(errno) );
				}
			
			}else if( is_dir_exist( temp_arr ) == 0 ){
				if( init_path( vol_ip, BASE_NEEDY ) < 0 ) //path is not initialised
					return -1; //else path is initialised
		
				if( chdir( temp_arr ) != 0 ){
					fprintf(stderr,"%s[%d]: Error %d in changing dir to %s >%s\n",__FILE__,__LINE__,errno,temp_arr, strerror(errno) );
				}
			}			
			// ============ put into ipcwd ============= 
			if( getcwd(cwd, sizeof(cwd)) == NULL ){
				fprintf(stderr,"%s[%d]: Error %d in getting cwd %s\n",__FILE__,__LINE__, errno, strerror(errno) );
			}
			ipcwd_insert_pair( ipcwd, vol_ip, cwd );
			
		}
		//delete previous dir on needy side /tmp/vol/ip/output
		else if( strcmp( sender_msg, "__INIT_DELETE_DIR_VOL__") == 0 ){			
			printf("received order: __INIT_DELETE_DIR_VOL__ from: %s\n", vol_ip);
			sprintf( temp_arr, BASE_VOL "/%s", vol_ip );
			if( is_dir_exist( temp_arr ) == 1 ){
				//remove dir below /tmp/needy/vol_ip <---also get removed
				remove_dir( temp_arr );
				init_path( vol_ip, BASE_VOL );

				if( chdir( temp_arr ) != 0 ){
					fprintf(stderr,"%s[%d]: Error %d in changing dir to %s >%s\n",__FILE__,__LINE__,errno,temp_arr, strerror(errno) );
				}
				
			}else if( is_dir_exist( temp_arr ) == 0 ){
				if( init_path( vol_ip, BASE_VOL ) < 0 ) //path is not initialised
					return -1; 
				//else path is initialised and we can proceed
			
				if( chdir( temp_arr ) != 0 ){
					fprintf(stderr,"%s[%d]: Error %d in changing dir to %s >%s\n",__FILE__,__LINE__,errno,temp_arr, strerror(errno) );
				}
			}
			//========= if no entry <ip,cwd> found then ===========
			//========= put cwd in ipcwd( needy side ) ==============
			if( getcwd(cwd, sizeof(cwd)) == NULL ){
				fprintf(stderr,"%s[%d]: Error %d in getting cwd %s\n",__FILE__,__LINE__, errno, strerror(errno) );
			}
			ipcwd_insert_pair( ipcwd, vol_ip, cwd );
		}
		//move one dir higher
		else if( strcmp(sender_msg,"__DIR_UP__") == 0 ){
			printf("received order: __DIR_UP__ from: %s\n", vol_ip);			
			//============= get cwd from ipcwd; not function below and use that as cwd==============
			strcpy( cwd, find_cwd( ipcwd, vol_ip) );
			char *pch = strrchr( cwd, '/');
			cwd[ pch-cwd ] = '\0';

			if( chdir( cwd ) != 0 ){
				fprintf(stderr,"%s[%d]: Error %d in going one dir higher to %s >%s\n",__FILE__,__LINE__, errno, cwd, strerror(errno) );
			}
			if( getcwd(cwd, sizeof(cwd)) == NULL ){
				fprintf(stderr,"%s[%d]: Error %d in getting cwd %s\n",__FILE__,__LINE__, errno, strerror(errno) );
			}
			ipcwd_insert_pair( ipcwd, vol_ip, cwd );
			// ========== update cwd in ipcwd ==============
		}
		//close opened file
		else if( strcmp(sender_msg,"__FILE_CLOSE__") == 0 ){
			//============= remove entry from ipfd ===========
			printf("received order: __FILE_CLOSE__ from: %s\n", vol_ip);
			fd = find_fd( ipfd, vol_ip );
			if( fd > 0 ){
				close( fd );
				ipfd_rem_pair( ipfd, vol_ip );
			}
		}
		//order to abort current task
		else if( strcmp(sender_msg,"__ABORT__") == 0 ){
			printf("received order: __ABORT__ from: %s\n", vol_ip);
			//close the running script corresponding to given ip_addr
			//========== close fd corresponding to ip and remove entry from ipfd ===========
			fd = find_fd( ipfd, vol_ip );
			printf("got fd: %d", fd );
			if( fd > 0 ){
				close(fd);
				ipfd_rem_pair( ipfd, vol_ip );
			}
			//remove the dir /tmp/needy/ip			
			sprintf( temp_arr, BASE_NEEDY"/%s", vol_ip );
			printf("removing dir %s\n", temp_arr );
			if( is_dir_exist( temp_arr ) == 1 ){
				//remove dir below /tmp/needy/vol_ip <---also get removed
				remove_dir( temp_arr );				
				ipcwd_rem_pair( ipcwd, vol_ip );
			}			 			
			//============== remove entry from ipcwd ================
			send_msg( vol_ip, "__FINISH__");
			show_entries();
		}
		//if finish signal is got
		else if( strcmp( sender_msg,"__FINISH__" ) == 0 ){
			printf("received order: __FINISH__ from: %s\n", vol_ip);
			//**************** -------------->remove entry from volunteer list and show finishing dialog box <--------------
			//************** disable the abort button and enable remove button***************
			printf("Ending communication with volunteer\n");
			// ========== remove entry from ipcwd(on needy side ) ===========
			fd = find_fd( ipfd, vol_ip );
			if( fd > 0 ){
				close( fd );
				ipfd_rem_pair( ipfd, vol_ip );
			}
			ipcwd_rem_pair( ipcwd, vol_ip );
			show_entries( );
		}
		else if( strstr( sender_msg,"__DONE_SENDING_DATA__") != NULL ){

			printf("received order: __DONE_SENDING_DATA__ from: %s\n", vol_ip);
			sprintf( temp_arr, BASE_NEEDY"/%s", vol_ip );
			
			//get start_dir name
			char start_dir[MAX_DIR_NAME_SIZE];
			char *pch = strstr( sender_msg,"__DONE_SENDING_DATA__");
			strcpy( start_dir, pch+22 );			
			//sprintf( start_dir,"%s", pch+22);
			
			printf("start dir: %s\n", start_dir );
			
			if( is_dir_exist(temp_arr) == 1 ){
				//scripts dir doesn't exist; nothing to do; may be delete data from given vol_ip
				char temp_arr2[ MAX_DIR_SIZE ];
				//here name of the root folder needs to be added
				//cwd is also not helpful as it is BASE now
				sprintf( temp_arr2, BASE_NEEDY"/%s/%s/scripts", vol_ip, start_dir );
				// prints till here				
				printf("temp_arr2 %s\n", temp_arr2);				
				if( is_dir_exist( temp_arr2 ) == 0 ){
					
					// ----------> no need to print it but reply back to needy
					printf("scripts dir doesn't exist\n");
					//========== __NO_SCRIPT_DIR__FOUND__
					send_msg( vol_ip, "__NO_SCRIPT_DIR_FOUND__");
					//==== remove the task dirs ====
					
				}else if( is_dir_exist( temp_arr2 ) == 1 ) {
					
					//for all script files in scripts dir feed each of them to system
					printf("trying to feed script %s\n", temp_arr2 );
					feed_scripts( temp_arr2 );
					//after scripts had run send back /tmp/needy/ip/start_dir
					sprintf( temp_arr2, BASE_NEEDY"/%s/%s/output", vol_ip, start_dir );
					send_result( temp_arr2, vol_ip );
					
				}else{
					printf("some weired error occured \n");
				}
								
			}else{
				//some error has occured in creating dir above scripts i.e. /tmp/needy/vol_ip
			}
				//======= check if there is any fd opened; close it; remove ipfd entry======
			fd = find_fd( ipfd, vol_ip );
			if( fd > 0 ){
				close(fd);	
				ipfd_rem_pair( ipfd, vol_ip );
			}								
			//======= remove entry from ipcwd =========
			ipcwd_rem_pair( ipcwd, vol_ip );
			
			show_entries();
		}
		//open file for writing
		else if( strstr( sender_msg, "__FILE__") != NULL ){

			printf("received order: __FILE__ from: %s\n", vol_ip);
			//======= check if file is already opened for ip =========
			//======= if fd is found; __PREV_ENTRY_FOUND_HTFD__  is returned
			fd = find_fd( ipfd, vol_ip );
			if( fd > 0 ){
				//send_msg( vol_ip, "__PREV_ENTRY_FOUND_IPFD__");
			}else{
				//get name of file
				char filename[MAX_DIR_NAME_SIZE];
				char *pch = strstr( sender_msg,"__FILE__");
				strcpy( filename, pch+9 );
				//check if file already exits if it exists open it otherwise create it and open it and overwrite it
				strcpy( cwd, find_cwd ( ipcwd, vol_ip ) );
				sprintf( temp_arr, "%s/%s", cwd, filename );
				printf("order to create file %s\n", temp_arr );
				process_mask = umask(0);	
				fd = open( temp_arr, O_CREAT | O_WRONLY, FILE_PERM );
				umask( process_mask );
				//======= insert <ip,fd> in ipfd
				
				ipfd_insert_pair( ipfd, vol_ip, fd );
			}
			
		}
		//create a dir and cd to it
		else if( strstr( sender_msg,"__DIR__") != NULL ){
			printf("received order: __DIR__ from: %s\n", vol_ip);
			//retrieve name of dir
			char dirname[MAX_DIR_NAME_SIZE];
			char *pch = strstr( sender_msg,"__DIR__");
			strcpy( dirname, pch+8 );
			
			//check for cwd if error
			//========= get cwd from ipcwd =======
			strcpy( cwd, find_cwd(ipcwd, vol_ip) );			
			sprintf( temp_arr, "%s/%s", cwd,dirname );
			printf("order to create dir with path %s\n", temp_arr );
			if( is_dir_exist( temp_arr) == 1 ){
					//dir exist just cd to it
				if( chdir( temp_arr ) != 0 ){
					fprintf(stderr,"%s[%d]: Error %d in changing dir to %s >%s\n",__FILE__,__LINE__,errno,temp_arr,strerror(errno) );
					return -1;
				}
			}else if( is_dir_exist( temp_arr) == 0 ){
					//dir doesn't exist
				process_mask = umask(0);
				if( mkdir(dirname,DIR_PERM) != 0 ){
					fprintf(stderr,"%s[%d]: Error %d in creating dir: %s >%s\n",__FILE__,__LINE__,errno, dirname, strerror(errno) );
				}
				umask( process_mask );
				
				if( chdir( temp_arr ) != 0 ){
					fprintf(stderr,"%s[%d]: Error %d in changing dir to %s >%s\n",__FILE__,__LINE__,errno,temp_arr,strerror(errno) );
					return -1;
				}
				
			}		
			
			if( getcwd(cwd, sizeof(cwd)) == NULL ){
				fprintf(stderr,"%s[%d]: Error %d in getting cwd >%s\n",__FILE__,__LINE__,errno, strerror(errno) );
			}
			printf("now cwd: %s\n", cwd );
			ipcwd_insert_pair( ipcwd, vol_ip, cwd );
			//======= update <ip, cwd> ==========
			
		}
	}
	
	close( sender_fd );
	free( vol_ip );
//	close( my_fd );
	return 0;
}

int est_con( ){

	int my_fd = socket( AF_INET, SOCK_STREAM, 0);
	if( my_fd == -1 ){
		fprintf(stderr,"Could not create socket\n");
		return -1;
	}
	
	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MY_PORT);
	char *my_ip_addr = get_my_ip_addr();	
	if( my_ip_addr == NULL ){
		printf("could not find your ip on eth0 interface. You must set MY_ADDR to your ip and recompile\n");
		my_addr.sin_addr.s_addr = inet_addr( MY_ADDR );	
	}else{
		my_addr.sin_addr.s_addr = inet_addr( my_ip_addr );// could also use INADDR_ANY
	}	
	memset( &(my_addr.sin_zero),'\0', 8 );

	if( bind(my_fd, (struct sockaddr*)&my_addr, sizeof(my_addr) ) < 0 ){
		fprintf(stderr,"Could not bind to port\n");
		return -2;
	}
	
	listen( my_fd, MAX_ALLOWED_CLIENTS );
//
	printf("Waiting for sender!\n");
//
	
	return my_fd;	
}

int main( ){
	
	int fd = est_con();
	ipfd = create_hash_table();
	ipcwd = create_hash_table();
	//when it starts it must multicast __FINISH__ which should be received by 
	//watchman to delete any previous ipfd and ipcwd entries which is not possible
	//but at least it can send __FINISH__ to volunteer list after reading 
	//file /tmp/volunteer.txt 
	while(1){
		receive_msg( fd );	//lets run it without breaking : it may cause very weired errors			
	}
	return 0;
}
