## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src 

distappdocdir = ${prefix}/doc/distapp
distappdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(distappdoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(distappdocdir)
