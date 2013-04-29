/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * distapp.c
 * Copyright (C) 2013 sherlock <sherlock@sherlock-laptop>
 * 
 * distapp is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * distapp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include<errno.h>
#include<unistd.h>

#include "distapp.h"
#include "gui.h"
#include "multicaster.h"
#include "dir_op.h"
#include "dir_sender.h"
#include "sender.h"
#include "dialog.h"
#include "defs.h"

/* For testing propose use the local (not installed) ui file */
/* #define UI_FILE PACKAGE_DATA_DIR"/ui/distapp.ui" */

#define UI_FILE TERMINAL_SRC"distapp.ui"
#define TOP_WINDOW "window"


G_DEFINE_TYPE (Distapp, distapp, GTK_TYPE_APPLICATION);


/* Define the private structure in the .c file */
#define DISTAPP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), DISTAPP_TYPE_APPLICATION, DistappPrivate))

struct _DistappPrivate
{
	/* ANJUTA: Widgets declaration for distapp.ui - DO NOT REMOVE */
};

//
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
//

/* Create a new window loading a file */
static void
distapp_new_window (GApplication *app,
                           GFile        *file)
{
	GtkWidget *window;

	GtkBuilder *builder;
	GError* error = NULL;

	DistappPrivate *priv = DISTAPP_GET_PRIVATE(app);

	/* Load UI from file */
	builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, UI_FILE, &error))
	{
		g_critical ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}

	/* Auto-connect signal handlers */
	gtk_builder_connect_signals (builder, app);

	/* Get the window object from the ui file */
	window = GTK_WIDGET (gtk_builder_get_object (builder, TOP_WINDOW));
        if (!window)
        {
		g_critical ("Widget \"%s\" is missing in file %s.",
				TOP_WINDOW,
				UI_FILE);
        }

	mydata.vol_viewport = GTK_VIEWPORT( gtk_builder_get_object (builder, VOL_VIEWPORT) );
	//mydata.done_viewport = GTK_VIEWPORT( gtk_builder_get_object (builder, DONE_VIEWPORT) );
	mydata.main_window = window;
		
	mydata.vbox = gtk_vbox_new ( FALSE, 0 );
	gtk_container_add ( GTK_CONTAINER( mydata.vol_viewport ), mydata.vbox );
	
	/* ANJUTA: Widgets initialization for distapp.ui - DO NOT REMOVE */
	
	g_object_unref (builder);
	
	
	gtk_window_set_application (GTK_WINDOW (window), GTK_APPLICATION (app));
	if (file != NULL)
	{
		/* TODO: Add code here to open the file in the new window */
	}

	//add_to_iplist("172.17.10.5");
	//add_to_iplist("172.17.9.17");
	//add_to_iplist("172.18.9.119");
	
	gtk_widget_show_all (GTK_WIDGET (window));	
	
	//*************** trying to run demons ******************* 
	//don't know it should be in while(1) loop or not; test it
	//multicast_watchman();
	//
	
	//
	//load the deny list
	char temp[SHMSIZE];
	FILE *ifp = fopen(VOL_DENY_LIST,"r");
	if( ifp == NULL ){
		fprintf(stderr, "%s[%d]: ERROR %d: Unable to open vol.deny list >%s\n",__FILE__,__LINE__,errno, strerror(errno) );
	}else{
		while( fgets( temp, SHMSIZE, ifp ) != NULL ){			
			char *ptr = (char *)malloc( strlen( temp ) );
			strcpy( ptr, temp );
			deny_list[dl_size] = ptr;
			dl_size++;
		}
	}
	//
}

void on_send_button_clicked( GtkWidget *widget, gpointer data ){
	
	Info *info = (Info *)data;
	//g_print("send button clicked for %s\n", info -> ip );	
	//disable send button and path entry and enable abort button
	gtk_widget_set_sensitive ( info -> send_button, FALSE );
	gtk_widget_set_sensitive ( info -> path_entry, FALSE );
	gtk_widget_set_sensitive ( info -> path_button, FALSE );
	gtk_widget_set_sensitive ( info -> abort_button, TRUE );
	gtk_widget_set_sensitive ( info -> rem_button, TRUE );
	char *path = gtk_entry_get_text ( info -> path_entry );
	char *ip = info -> ip;
	
	//if path exists
	if( is_dir_exist ( path ) > 0 ){
		//send the dir
		send_dir( path, ip );
	}
	//otherwise path given is NULL
	//show message to give a valid path
	else{
		show_error_dialog ("Please enter a valid path" );
		gtk_widget_set_sensitive ( info -> send_button, TRUE );
		
		gtk_widget_set_sensitive ( info -> path_entry, TRUE );
		gtk_entry_set_text ( info -> path_entry, "" );
		
		gtk_widget_set_sensitive ( info -> path_button, TRUE );
		gtk_widget_set_sensitive ( info -> abort_button, FALSE );
		gtk_widget_set_sensitive ( info -> rem_button, TRUE );
	}
}

void on_abort_button_clicked( GtkWidget *widget, gpointer data ){
	Info *info = (Info *)data;
	//g_print("abort button clicked for %s\n", info -> ip );
	//send abort signal to volunteer
	
	if( send_msg ( info -> ip, "__ABORT__" ) == 0 ){
		gtk_widget_set_sensitive ( info -> abort_button, FALSE );	
		gtk_widget_set_sensitive ( info -> rem_button, TRUE );
	}

	
	
}

void on_rem_button_clicked( GtkWidget *widget, gpointer data ){
	Info *info = (Info *)data;
	//g_print("remove button clicked for %s\n", info -> ip );
	//LET rem_button remain active
	//gtk_widget_set_sensitive ( info -> rem_button, FALSE );
	/*
	 * some error in removing dir so we shall not remove it
	 * is_dir_exist(path) fails due to unknown reasons
	char temp_arr[MAX_DIR_SIZE];
	sprintf( temp_arr, BASE_VOL "/%s", g_strdup( gtk_label_get_text (info -> ip_label ) ) );
	g_print("trying to remove dir : %s; len : %d\n", temp_arr, strlen(temp_arr) );
	int ret = is_dir_exist( temp_arr ); 
	if( ret == 1 ){
		remove_dir ( temp_arr );
		g_print("removed dir : %s\n", temp_arr );
	}else if( ret == 0){		
		g_print("ret = 0; dir : %s doesn't exists\n", temp_arr );
	}else{
		g_print("ret: %d; and some weired error occured\n", ret );
	}
	
	remove_dir ( temp_arr );
		//remove the entry from list
	*/
	//gtk_widget_hide ( info -> hbox );
	gtk_widget_destroy ( info -> hbox );
	
}

void on_path_button_clicked( GtkWidget *widget, gpointer data ){
	
	Info *info = (Info *)data;
//
	 GtkDialog *dialog;     
     dialog = gtk_file_chooser_dialog_new ("Select Folder",
     				      mydata.main_window,
     				      GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
     				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
     				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
     				      NULL);
     
     if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
		 
         gchar *path;     
         path = gtk_file_chooser_get_uri (GTK_FILE_CHOOSER (dialog));
		 gtk_entry_set_text ( info -> path_entry, path + 7 );
         g_free ( path );
		 
     }     
     gtk_widget_destroy (dialog);
//	
	
}

GtkBox* make_hbox( gchar *ip ){

	//printf("called make_hbox\n");
	
	GtkBox *box = NULL;	
	GtkButton *path_button;
	GtkButton *send_button;
	GtkButton *abort_button;
	GtkButton *rem_button;
	GtkLabel *ip_label;
	GtkEntry *path_entry;
	GtkSeparator *vsep;
	
	box = gtk_hbox_new ( FALSE, 0 );
	ip_label = gtk_label_new ( g_strdup( ip ) );
	gtk_box_pack_start (  box , ip_label, FALSE, FALSE, 3 );
	gtk_widget_show ( ip_label );
/*
	vsep = gtk_vseparator_new ();
	gtk_box_pack_start (  box , vsep, FALSE, FALSE, 5 );
	gtk_widget_show ( vsep );
*/	
	path_entry = gtk_entry_new ( );
	gtk_box_pack_start ( box, path_entry, FALSE, FALSE, 3 );
	gtk_widget_show( path_entry );	

	path_button = gtk_button_new_with_label ("Select Directory");
	gtk_box_pack_start ( box, path_button, FALSE, FALSE, 3 );
	gtk_widget_show( path_button );	
	
	send_button = gtk_button_new_with_label ("Send");
	gtk_box_pack_start( box, send_button, FALSE, FALSE, 3 );
	gtk_widget_show( send_button );	 		
	
	abort_button = gtk_button_new_with_label ("Abort");
	gtk_box_pack_start( box, abort_button, FALSE, FALSE, 3 );
	gtk_widget_set_sensitive ( abort_button, FALSE );
	gtk_widget_show ( abort_button );
	
	rem_button = gtk_button_new_with_label ("Remove");
	gtk_box_pack_start( box, rem_button, FALSE, FALSE, 3 );
	gtk_widget_set_sensitive ( rem_button, TRUE );
	gtk_widget_show ( rem_button );
	
	Info *info = (Info *)malloc( sizeof(Info) );
	info -> ip_label = ip_label;
	info -> ip = (char *)malloc( strlen( ip ) + 1 );
	info -> ip = g_strdup( ip ); 
	info -> path_entry = path_entry;
	info -> path_button = path_button;
	info -> send_button = send_button;
	info -> abort_button = abort_button;
	info -> rem_button = rem_button;
	
	g_signal_connect( abort_button, "clicked", G_CALLBACK(on_abort_button_clicked), (gpointer)info );
	g_signal_connect( send_button, "clicked", G_CALLBACK(on_send_button_clicked), (gpointer)info );
	g_signal_connect( rem_button, "clicked", G_CALLBACK(on_rem_button_clicked), (gpointer)info );
	g_signal_connect( path_button, "clicked", G_CALLBACK(on_path_button_clicked), (gpointer)info );
	
	info -> hbox = box;
	//printf("returning from make_hbox\n");
	return box;
	
}

void add_to_iplist ( gchar *ip ){	

	GtkSeparator *hsep;		
	if( mydata.vbox == NULL )
		mydata.vbox = gtk_vbox_new ( FALSE, 0 );

	GtkBox *hbox = make_hbox( ip );		
	
	gtk_box_pack_start ( mydata.vbox, hbox, FALSE, TRUE, 5 );
/*
	hsep = gtk_hseparator_new ();
	gtk_box_pack_start (  mydata.vbox , hsep, FALSE, FALSE, 7 );
	gtk_widget_show ( hsep );
*/	
	gtk_widget_show( hbox );
	gtk_widget_show ( mydata.vbox );	
	
}


/* GApplication implementation */
static void
distapp_activate (GApplication *application)
{
	distapp_new_window (application, NULL);
}

static void
distapp_open (GApplication  *application,
                     GFile        **files,
                     gint           n_files,
                     const gchar   *hint)
{
	gint i;

	for (i = 0; i < n_files; i++)
		distapp_new_window (application, files[i]);
}

//static void "wrote void instead of static void below"
void distapp_init (Distapp *object)
{

}

static void
distapp_finalize (GObject *object)
{
	G_OBJECT_CLASS (distapp_parent_class)->finalize (object);
}

static void
distapp_class_init (DistappClass *klass)
{
	G_APPLICATION_CLASS (klass)->activate = distapp_activate;
	G_APPLICATION_CLASS (klass)->open = distapp_open;

	g_type_class_add_private (klass, sizeof (DistappPrivate));

	G_OBJECT_CLASS (klass)->finalize = distapp_finalize;
}

Distapp *
distapp_new (void)
{
	g_type_init ();

	return g_object_new (distapp_get_type (),
	                     "application-id", "org.gnome.distapp",
	                     "flags", G_APPLICATION_HANDLES_OPEN,
	                     NULL);
}


void
on_navigate_clicked (GtkToolButton *toolbutton, gpointer user_data)
{

	g_print("on navigate clicked\n");
	//using multicaster multicast
	if( truncate(VOLUNTEER_TXT_PATH, 0 ) == -1 ){
		fprintf(stderr,"%s[%d]: Error %d in truncating file "VOLUNTEER_TXT_PATH" >%s\n",__FILE__,__LINE__,errno, strerror(errno) );
	}
	multicast();
	gtk_widget_set_sensitive ( (GtkToolButton *)user_data, TRUE );
	
}


void
on_refresh_clicked (GtkToolButton *toolbutton, gpointer user_data)
{

	gtk_widget_set_sensitive ( toolbutton, FALSE );
	char ip[SHMSIZE];
	FILE *ifp = fopen( VOLUNTEER_TXT_PATH, "r" );
	if( ifp == NULL ){
		fprintf(stderr,"%s[%d]: Error %d in opening file "VOLUNTEER_TXT_PATH" >%s\n",__FILE__,__LINE__,errno, strerror(errno) );
	}else{
		while( fgets( ip, SHMSIZE, ifp ) != NULL ){
			if( is_denied( ip ) == 0 )
				add_to_iplist( ip );
			//otherwise ip is not allowed to act as volunteer
		}
	}
	
}

