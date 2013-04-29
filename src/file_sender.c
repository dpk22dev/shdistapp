#include "headers.h"
#include "file_sender.h"

int send_file( char *ip, char *filename ){

	int fd,rc;
	struct stat stat_buf;
	off_t offset = 0;

	if( ip == NULL ){
		fprintf (stderr, "Could not find destination address\n");
		return -1;
	}		
	
	int sender_fd = socket( AF_INET, SOCK_STREAM, 0 );
	if( sender_fd == -1 ){
		fprintf (stderr,"Could not create socket for sender\n");
		return -1;
	}
	printf("file_sender.c: created socket\n");
	struct sockaddr_in receiver;
	receiver.sin_family = AF_INET;
	receiver.sin_port = htons( RECEIVER_PORT );
	receiver.sin_addr.s_addr = inet_addr( ip );
	memset( &(receiver.sin_zero),'\0', 8 );

	if( connect(sender_fd, (struct sockaddr*)&receiver, sizeof(receiver)) < 0 ){
		fprintf (stderr, "Could not connect to receiver\n");
		return -2;
	}
	printf("file_sender.c: connection established\n");
	
	if( filename == NULL ){
		fprintf(stderr,"File name not given\n");
		return -3;
	}
	fd = open( filename, O_RDONLY );
	if( fd == -1 ){	
		fprintf( stderr,"unable to open send file\n" );
		return -4;
	}	
	
	fstat( fd, &stat_buf );
	offset = 0;

	rc = sendfile( sender_fd, fd, &offset, stat_buf.st_size );
	if( rc == -1 ){
		fprintf(stderr,"error from sendfile %s\n", strerror(errno));
		return -5;
	}
	if( rc != stat_buf.st_size ){
		fprintf(stderr,"incomplete transfer of file %d of %d bytes\n", rc, (int)stat_buf.st_size);
		return -6;
	}
/*
	if( send(sender_fd, data, strlen(data), 0) < 0 ){
		fprintf ("Could not send data\n");
		return -3;
	}

*/
	printf("file_sender.c: file sent\n");
	close( sender_fd );
	
	return 0;
}

/*
int main( ){
	send_file("172.17.10.5", "/home/pc/Desktop/tasks");
}
*/
