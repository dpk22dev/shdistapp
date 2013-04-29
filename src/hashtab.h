#ifndef HASHTAB_H
#define HASHTAB_H

#include <glib.h>

GHashTable* create_hash_table( );
gboolean does_ip_exist( GHashTable *hash, char *ip );
void ipfd_insert_pair( GHashTable *hash, char *ip, int fd );
void ipcwd_insert_pair( GHashTable *hash, char *ip, char *cwd );
int find_fd( GHashTable *hash, char *ip );
char* find_cwd( GHashTable *hash, char *ip );
gboolean ipfd_rem_pair( GHashTable *hash, char *ip );
gboolean ipcwd_rem_pair( GHashTable *hash, char *ip );
void ipfd_show_pair( gpointer key, gpointer value, gpointer data );
void ipfd_show_all_entries( GHashTable *hash );
void ipcwd_show_pair( gpointer key, gpointer value, gpointer data );
void ipcwd_show_all_entries( GHashTable *hash );

#endif
