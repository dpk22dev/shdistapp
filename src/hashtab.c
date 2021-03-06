#include<stdio.h>
#include<glib.h>
#include "hashtab.h"

//may be we should use g_hash_table_replace for updation of key value pair

GHashTable* create_hash_table( ){
	GHashTable *hash = g_hash_table_new_full( g_str_hash, g_str_equal, g_free, g_free );
	return hash;	
}

gboolean does_ip_exist( GHashTable *hash, char *ip ){
	
	gboolean ret;
	gchar *ptr_ip = g_strdup( ip );
	//ret = g_hash_table_contains( hash, ptr_ip );	
	
	ret = FALSE;
	GHashTableIter iter;
	gpointer key, value;
	g_hash_table_iter_init( &iter, hash );
	while( g_hash_table_iter_next( &iter, &key, &value ) ){
	 	if( g_strcmp0( key, ptr_ip ) == 0 ){
			 ret = TRUE;  	
			 break;
		}				   
	}
	g_free( ptr_ip );
	return ret;
	
}

void ipfd_insert_pair( GHashTable *hash, char *ip, int fd ){
	gint *ptr_fd = g_new( gint, 1 );
	*ptr_fd = fd;
	gchar *ptr_ip = g_strdup( ip );	
	g_hash_table_insert( hash, ptr_ip, ptr_fd );
}

void ipcwd_insert_pair( GHashTable *hash, char *ip, char *cwd ){
	gchar *ptr_ip = g_strdup( ip );
	gchar *ptr_cwd = g_strdup( cwd );
	g_hash_table_insert( hash, ptr_ip, ptr_cwd );
}


gboolean ipfd_rem_pair( GHashTable *hash, char *ip ){

	gboolean ret = FALSE;
	gchar *ptr_ip = g_strdup( ip );
	if( does_ip_exist ( hash, ptr_ip ) == FALSE ){
		ret = FALSE;
	}else{
		ret = g_hash_table_remove( hash, ptr_ip );
	}
	g_free( ptr_ip );
	return ret;
	
}

gboolean ipcwd_rem_pair( GHashTable *hash, char *ip ){

	gboolean ret = FALSE;
	gchar *ptr_ip = g_strdup( ip );
	if( does_ip_exist( hash, ptr_ip ) == FALSE ){
		ret = FALSE;
	}else{
		ret = g_hash_table_remove( hash, ptr_ip );
	}
	g_free( ptr_ip );
	return ret;
	
}


int find_fd( GHashTable *hash, char *ip ){

	gchar *ptr_ip = g_strdup( ip );
	gint *ptr_fd, ret = -1;
	if( g_hash_table_lookup( hash, ptr_ip ) != NULL ){
		ptr_fd = g_hash_table_lookup( hash, ptr_ip );
		ret = *ptr_fd;
	}
	g_free( ptr_ip );
	return ret; 
	
}

char* find_cwd( GHashTable *hash, char *ip ){
	
	gchar *ptr_ip = g_strdup( ip );
	gchar *ptr_cwd = NULL, *temp;
	if( g_hash_table_lookup( hash, ptr_ip ) != NULL )
		ptr_cwd = g_strdup( g_hash_table_lookup( hash, ptr_ip ) );//may be it must be g_strdup( g_hash... )
	g_free( ptr_ip );
	return ptr_cwd;
	
}

void ipfd_show_pair( gpointer key, gpointer value, gpointer data ){
	printf("key: %s  value: %d\n", (gchar *)key,*(gint *)value );
}

void ipfd_show_all_entries( GHashTable *hash ){
	g_hash_table_foreach(hash, (GHFunc)ipfd_show_pair, NULL );
}

void ipcwd_show_pair( gpointer key, gpointer value, gpointer data ){
	printf("key: %s  value: %s\n", (gchar *)key,(gchar *)value );
}

void ipcwd_show_all_entries( GHashTable *hash ){
	g_hash_table_foreach(hash, (GHFunc)ipcwd_show_pair, NULL );
}
