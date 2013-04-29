#ifndef CALC_LOAD_H
#define CALC_LOAD_H

#include<stdio.h>
#include<string.h>

#define MAX_BYTES_READ 1024

int get_number_cores( );
int get_avg_load( float *minute, float* five_minute, float* fifteen_minute );

#endif