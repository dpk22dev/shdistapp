#include "multicaster.h"

int multicast( )
{
	//printf("******multicasting******\n");
     struct sockaddr_in addr;
     int fd, cnt;
     struct ip_mreq mreq;
     char *message="__HELP__";

	if( ( fd = socket( AF_INET, SOCK_DGRAM, 0) ) < 0 ){
		perror("failure socket\n");
		exit(1);
	}		

	memset( &addr, 0, sizeof(addr) );
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr=inet_addr(HELP_GROUP);
    addr.sin_port=htons(HELP_PORT);	

	//may be this while should not be commeted out
//	while(1){
		if( sendto( fd, message, strlen(message)+1, 0, (struct sockaddr*)&addr,sizeof(addr) ) < 0 ){
			perror("sendto failure\n");
			exit(1);
		}
		return 1;
//	}
	
}

