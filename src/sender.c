#include "sender.h"
#include "dialog.h"

int send_msg( char *ip, char *data ){

	if( ip == NULL ){
		fprintf(stderr,"Could not find destination address\n");
		return -1;
	}		
	
	int sender_fd = socket( AF_INET, SOCK_STREAM, 0 );
	if( sender_fd == -1 ){
		fprintf(stderr,"Could not create socket for sender\n");
		return -1;
	}
	printf("sender.c: created socket\n");
	struct sockaddr_in receiver;
	receiver.sin_family = AF_INET;
	receiver.sin_port = htons( RECEIVER_PORT );
	receiver.sin_addr.s_addr = inet_addr( ip );
	memset( &(receiver.sin_zero),'\0', 8 );

	if( connect(sender_fd, (struct sockaddr*)&receiver, sizeof(receiver)) < 0 ){
		fprintf(stderr,"Could not connect to receiver\n");
		return -2;
	}
	printf("sender.c: connection established\n");
	
	if( data == NULL )
		data = "Hello world from sender\n";	
		
	if( send(sender_fd, data, strlen(data), 0) < 0 ){
		fprintf(stderr,"Could not send data\n");
		return -3;
	}
	printf("sender.c: data sent\n");
	close( sender_fd );
	
	return 0;
}