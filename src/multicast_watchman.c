/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2013 sherlock <sherlock@sherlock-laptop>
 * 
 * sem_proj is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * sem_proj is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "headers.h"
#include "calc_load.h"
#include "sender.h"
#include "defs.h"				

char *deny_list[MAX_DENY_LIST_SIZE];
int dl_size = 0;

int is_denied( char *ip ){
	int i = 0, ret = 0;
	for( i = 0; i < dl_size; i++ ){
		if( strcmp( deny_list[i], ip ) == 0 ){
			ret = 1;
			break;
		}
	}
	return ret;
}

void *cal_reply( void *multicaster_ip ){

	char *sender_ip = (char *)multicaster_ip;
	//calculate load and reply back
	int cores = get_number_cores ( );	
	//printf("#cores found: %d\n", cores );
	
	float *minute = (float *)malloc( sizeof(float) );
	float *five_minute = (float *)malloc( sizeof(float) );
	float *fifteen_minute = (float *)malloc( sizeof(float) );
	float minute_avg, five_minute_avg, fifteen_minute_avg;
	float policy_minute_avg = 50, policy_five_minute_avg = 50, policy_fifteen_minute_avg = 50;
	//be careful average load can be greater than 100% so truncate it

	if( get_avg_load ( minute, five_minute, fifteen_minute) ){
		minute_avg = (*minute)*100.0/cores;
		five_minute_avg = (*five_minute)*100.0/cores;
		fifteen_minute_avg = (*fifteen_minute)*100.0/cores;
		//printf("last minute avg load: %f\n", ( (*minute)*100.0/cores < 100 ) ? ( (*minute)*100.0/cores ) : 100 );
		//printf("last five minute avg load: %f\n", ( (*five_minute)*100.0/cores < 100 ) ? ( (*five_minute)*100.0/cores ) : 100 );
		//printf("last fifteen minute avg load: %f\n", ( (*fifteen_minute)*100.0/cores < 100 ) ? ( (*fifteen_minute)*100.0/cores ) : 100 );
		//printf("applying policy\n");
		FILE *ifp = fopen(POLICY_TXT,POLICY_TXT_MODE);
		if( ifp == NULL ){
			perror ("could not open policy file" );
			printf("using default policy of 50 50 50");			
		}else{
			fscanf(ifp, "%f %f %f", &policy_minute_avg, &policy_five_minute_avg, &policy_fifteen_minute_avg );
			fclose (ifp);
		}
		//instead make sender a separate process or thread
		char reply[30];
		if( minute_avg < policy_minute_avg && five_minute_avg < policy_five_minute_avg && fifteen_minute_avg < policy_fifteen_minute_avg ){
			strcpy( reply, "__READY_TO_HELP__");					
		}else{
			strcpy( reply,"__NOT_READY_TO_HELP__");
		}
	
		if( send_msg ( sender_ip, reply ) == 0 ){
			printf("replied: %s\n", reply );
		}else{
			printf("error in replying back to needy\n");
		}
				
	}else{
		fprintf(stderr, "%s[%d]: ERROR %d: calculate_avg_load >%s\n",__FILE__,__LINE__, errno, strerror(errno) );
		exit(EXIT_FAILURE);
	}
	
	FILE *ofp;
	ofp = fopen( NEEDY_TXT, NEEDY_TXT_MODE );		
	if( ofp == NULL ){
		printf("error in opening output file: %s\n" , NEEDY_TXT );
		exit(1);
	}
	fprintf( ofp, "needy: %s\n", sender_ip );
	fclose( ofp );	
	
}

//void multicast_watchman( )
int  main( )
{
	//
	//load the deny list
	char temp[SHMSIZE];
	FILE *ifp = fopen(NEEDY_DENY_LIST,"r");
	if( ifp == NULL ){
		fprintf(stderr, "%s[%d]: ERROR %d: Unable to open needy.deny list >%s\n",__FILE__,__LINE__,errno, strerror(errno) );
	}else{
		while( fgets( temp, SHMSIZE, ifp ) != NULL ){			
			char *ptr = (char *)malloc( strlen( temp ) );
			strcpy( ptr, temp );
			deny_list[dl_size] = ptr;
			dl_size++;
		}
	}
	//
	printf("multicast_watchman is running\n");
     struct sockaddr_in addr, sender_addr;
     int fd, nbytes,addrlen;
     struct ip_mreq mreq;
     char msgbuf[MSGBUFSIZE];

     u_int yes=1;            /*** MODIFICATION TO ORIGINAL */

     /* create what looks like an ordinary UDP socket */
     if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
	  perror("multicast_watchman.c: Could not create socket\n");
	  exit(1);
     }

	if( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes) ) < 0 ){
		perror("Could not reuse addr\n");
		exit(1);
	}

	memset( &addr, 0, sizeof(addr) );
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(HELP_PORT);

	if( bind( fd, (struct sockaddr *)&addr, sizeof(addr) ) < 0 ){
		perror("Could not bind\n");
		exit(1);
	}

	mreq.imr_multiaddr.s_addr = inet_addr( HELP_GROUP );
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if( setsockopt( fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0 ){
		perror("setsockopt kernel failure\n");
		exit(1);
	}

	//int counter = 0;
	while( 1 ){
		
		addrlen = sizeof( sender_addr );
		if( (nbytes = recvfrom( fd, msgbuf, MSGBUFSIZE, 0, (struct sockaddr*)&sender_addr, (socklen_t*)&addrlen ) )< 0 ){	
				perror("Could not recvfrom multicaster\n");
				exit(1);
		}		
		//printf("got msg: %s len: %d nbytes: %d\n", msgbuf, strlen(msgbuf), nbytes );
		
		if( strcmp(msgbuf,"__HELP__") == 0 ){
			
			//get sender ip
			char *sender_ip = malloc( strlen(inet_ntoa( sender_addr.sin_addr )) + 1);
			strcpy( sender_ip, inet_ntoa( sender_addr.sin_addr ) );												
			//create a new thread and send multicaster ip to it

			if( is_denied( sender_ip ) == 0 ){
			
				pthread_t thr;
				if( pthread_create( &thr, NULL, cal_reply, (void *)sender_ip ) < 0 ){
					fprintf(stderr, "%s[%d]: ERROR %d: unable to create thread >%s\n",__FILE__,__LINE__,errno, strerror(errno) );
					perror( NULL );
				}
				//write source ip address to file
				pthread_join( thr, NULL );
			}else{
				//else sender_ip is denied for help
				printf("replied: __NOT_READY_TO_HELP__ due to deny.list\n" );
				send_msg( sender_ip, "__NOT_READY_TO_HELP__" );
			}
			//printf("equal\n");			
			
//			fprintf( ofp,"counter: %d\n", counter );
			
		}else{
			//printf("Unequal\n" );	
			//printf("got msg: %s\n", msgbuf);
			//do nothing
		}
		//counter++;
		msgbuf[0] = '\0';
	}
	return 0;
}
