#include "calc_load.h"
#include <errno.h>
#include <stdlib.h>

int get_number_cores( ){

	FILE *ifp;
	char buff[MAX_BYTES_READ];
	char *ptr;
	size_t read_bytes;
	int num_cores;
	
	ifp = fopen("/proc/cpuinfo","r");
	if( ifp == NULL ){
		fprintf(stderr,"%s[%d]: ERROR %d: open file /proc/cpuinfo >%s\n",__FILE__,__LINE__, errno, strerror(errno));
		exit( EXIT_FAILURE );
	}
	
	read_bytes = fread( buff, 1, sizeof(buff), ifp );
	
	if( read_bytes == 0 ){
		fprintf(stderr,"%s[%d]: ERROR %d: could not read /proc/cpuinfo >%s\n",__FILE__,__LINE__, errno, strerror(errno));
		exit( EXIT_FAILURE );	
	}
	buff[read_bytes] = '\0';	//no more chars req
	
	ptr = strstr( buff, "cpu cores" );
	
	if( ptr == NULL ){
		fprintf(stderr,"Could not find \"cpu cores\" entry in /proc/cpuinfo\n" );
		exit( EXIT_FAILURE );
	}

	sscanf( ptr, "cpu cores	: %d", &num_cores );
	fclose( ifp );
	return num_cores;
	
}

int get_avg_load( float *minute, float* five_minute, float* fifteen_minute ){

	char buff[MAX_BYTES_READ];
	size_t read_bytes;
	
	FILE *ifp = fopen("/proc/loadavg","r");
	if( ifp == NULL ){
		fprintf(stderr,"%s[%d]: ERROR %d: could not open /proc/loadavg >%s\n",__FILE__,__LINE__, errno, strerror(errno));
		exit( EXIT_FAILURE );
	}
	
	read_bytes = fread( buff, 1, sizeof(buff), ifp );
	
	if( read_bytes == 0 ){
		fprintf(stderr,"Could not read /proc/cpuinfo\n");
		exit( EXIT_FAILURE );	
	}
	
	buff[read_bytes] = '\0';	//no more chars req
	if( sscanf(buff,"%f %f %f", minute, five_minute, fifteen_minute) == 3 )
		return 1;
	else{
		fprintf(stderr,"Could not correctly read averages\n");
		return 0;
	}
	
}
