#ifndef DIR_OP_H
#define DIR_OP_H

int init_path( char *dirname, char *base );
int is_dir_exist( char *path );
int remove_dir( char *path );
void iterate_dir( const char *path, const char *rec_ip );

#endif