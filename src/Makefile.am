## Process this file with automake to produce Makefile.in

## Created by Anjuta


uidir = $(pkgdatadir)/ui
ui_DATA = distapp.ui


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(DISTAPP_CFLAGS) 	

AM_CFLAGS =\
	 -Wall\
	 -g\
	-lpthread


bin_PROGRAMS = distapp \
	multicast_watchman \
	watchman

distapp_SOURCES = \
	main.c \
	distapp.h \
	distapp.c \
	gui.h \
	sender.h \
	sender.c \
	dir_sender.c \
	file_sender.c \
	file_sender.h \
	dir_sender.h \
	multicaster.c \
	multicaster.h \
	dir_op.c \
	dir_op.h \
	defs.h \
	dialog.c \
	dialog.h \
	vol.deny	

distapp_LDFLAGS = \
	-Wl,--export-dynamic

distapp_LDADD = $(DISTAPP_LIBS)

multicast_watchman_SOURCES =  \
	multicast_watchman.c \
	multicast_watchman.h \
	policy.txt \
	calc_load.c \
	calc_load.h \
	headers.h \
	sender.c \
	sender.h \
	defs.h \
	needy.deny

watchman_SOURCES =  \
	watchman.c \
	watchman.h \
	dir_sender.c \
	dir_sender.h \
	file_sender.c \
	file_sender.h \
	sender.c \
	sender.h \
	ip.h \
	ip.c \
	dir_op.h \
	dir_op.c \
	headers.h \
	scripts.c \
	scripts.h \
	defs.h \
	get_pid.c \
	get_pid.h \
	hashtab.c \
	hashtab.h

watchman_LDADD = $(DISTAPP_LIBS) 


EXTRA_DIST = $(ui_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(pkgdatadir)

