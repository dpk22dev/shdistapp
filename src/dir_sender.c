#include "headers.h"
#include "file_sender.h"
#include "sender.h"
#include "dir_op.h"

#define REC_IP "172.17.10.5"

//int main( ){
void send_dir( char *path, char *rec_ip )
{

	if( is_dir_exist( path ) < 1 ){
		fprintf(stderr,"%s[%d]: Error %d in sending dir %s\n>%s\n",__FILE__,__LINE__,errno, path, strerror(errno) );
		return ;
	}
	
	send_msg( rec_ip, "__INIT_DELETE_DIR_NEEDY__" );
	
	char base_dir[256], *cptr;
	cptr = strrchr( path, '/' );
	
//	tell to send base dir
	sprintf( base_dir, "__DIR__ %s", cptr+1 );
	send_msg( rec_ip, base_dir );
//
	printf("base_dir: %s\n", base_dir );
	//--------> uncomment below to start working ----------->
	iterate_dir( path, rec_ip );
	send_msg( rec_ip, "__DIR_UP__" );
	// sh after __DONE_SENDING_DATA__ is base dir name it must be retreived and sent along with
	//send_msg( rec_ip, "__DONE_SENDING_DATA__ sh");
	cptr = strrchr( path, '/' );	
	sprintf( base_dir, "__DONE_SENDING_DATA__ %s", cptr+1 );
	send_msg( rec_ip, base_dir );
		
}

void send_result( char *path, char *rec_ip ){

	if( is_dir_exist( path ) < 1 ){
		fprintf(stderr,"%s[%d]: Error %d in sending dir %s\n>%s\n",__FILE__,__LINE__,errno, path, strerror(errno) );
		return ;
	}
	
	send_msg( rec_ip, "__INIT_DELETE_DIR_VOL__" );
	
	char base_dir[256], *cptr;
	cptr = strrchr( path, '/' );
	
//	tell to send base dir
	sprintf( base_dir, "__DIR__ %s", cptr+1 );
	send_msg( rec_ip, base_dir );
//
	printf("base_dir: %s\n", base_dir );
	//--------> uncomment below to start working ----------->
	iterate_dir( path, rec_ip );
	send_msg( rec_ip, "__DIR_UP__" );
	// sh after __DONE_SENDING_DATA__ is base dir name it must be retreived and sent along with
	//send_msg( rec_ip, "__DONE_SENDING_DATA__ sh");
	//cptr = strrchr( path, '/' );	
	//sprintf( base_dir, "__DONE_SENDING_DATA__ %s", cptr+1 );
	send_msg( rec_ip, "__FINISH__" );
	
}