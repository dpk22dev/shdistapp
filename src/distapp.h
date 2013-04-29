/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * distapp.h
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

#ifndef _DISTAPP_
#define _DISTAPP_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DISTAPP_TYPE_APPLICATION             (distapp_get_type ())
#define DISTAPP_APPLICATION(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), DISTAPP_TYPE_APPLICATION, Distapp))
#define DISTAPP_APPLICATION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), DISTAPP_TYPE_APPLICATION, DistappClass))
#define DISTAPP_IS_APPLICATION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DISTAPP_TYPE_APPLICATION))
#define DISTAPP_IS_APPLICATION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), DISTAPP_TYPE_APPLICATION))
#define DISTAPP_APPLICATION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), DISTAPP_TYPE_APPLICATION, DistappClass))

typedef struct _DistappClass DistappClass;
typedef struct _Distapp Distapp;
typedef struct _DistappPrivate DistappPrivate;

struct _DistappClass
{
	GtkApplicationClass parent_class;
};

struct _Distapp
{
	GtkApplication parent_instance;

	DistappPrivate *priv;

};

typedef struct Info{
	gchar *ip;
	GtkLabel *ip_label;
	GtkEntry *path_entry;
	GtkButton *path_button;
	GtkButton *send_button;
	GtkButton *abort_button;
	GtkButton *rem_button;
	GtkBox *hbox;
}Info;

GType distapp_get_type (void) G_GNUC_CONST;
Distapp *distapp_new (void);

/* Callbacks */
void on_refresh_clicked (GtkToolButton *toolbutton, gpointer user_data);

void on_navigate_clicked (GtkToolButton *toolbutton, gpointer user_data);


G_END_DECLS

void add_to_iplist( gchar *ip );
GtkBox* make_hbox ( gchar *ip );


#endif /* _APPLICATION_H_ */

