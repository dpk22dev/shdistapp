#include <gtk/gtk.h>

#include "dialog.h"
#include "gui.h"

void show_error( char *err ){
	
	fprintf(stderr,err);
	return ;
	
}

void show_error_dialog( char *err ){

	GtkDialog *dialog;
	dialog = gtk_message_dialog_new(GTK_WINDOW(mydata.main_window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            err);

	gtk_window_set_title(GTK_WINDOW(dialog), "Error");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	
}

void show_info_dialog( char *info )
{

	if( mydata.main_window == NULL ){
		printf("%s\n", info );
	}else{ 	
  		GtkWidget *dialog;
  		dialog = gtk_message_dialog_new(GTK_WINDOW(mydata.main_window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            info, "title");
  		gtk_window_set_title(GTK_WINDOW(dialog), "Information");
  		gtk_dialog_run(GTK_DIALOG(dialog));
  		gtk_widget_destroy(dialog);
 	}
	
}