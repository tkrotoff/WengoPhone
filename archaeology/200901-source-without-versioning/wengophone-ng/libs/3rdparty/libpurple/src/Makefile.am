EXTRA_DIST = \
		COPYRIGHT \
		ChangeLog.API \
		ChangeLog.win32 \
		Doxyfile.in \
		HACKING \
		Makefile.mingw \
		PLUGIN_HOWTO \
		README.MTN \
		README.mingw \
		config.h.mingw \
		gaim.pc.in \
		gaim-uninstalled.pc.in \
		intltool-extract.in \
		intltool-merge.in \
		intltool-update.in \
		pidgin.apspec.in \
		pidgin.spec.in \
		pidgin.desktop.in \
		po/Makefile.mingw \
		valgrind-suppressions

noinst_HEADERS = config.h

dist-hook: pidgin.spec
	cp pidgin.spec $(distdir)
	rm $(distdir)/config.h

distcheck-hook: libpurple/plugins/perl/common/Purple.pm pidgin/plugins/perl/common/Pidgin.pm
#	cp libpurple/plugins/perl/common/Gaim.pm $(distdir)/libpurple/plugins/perl/common

appsdir = $(datadir)/applications
apps_in_files = pidgin.desktop.in
apps_DATA = $(apps_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

if ENABLE_GTK
GTK_DIR=pidgin
endif

if ENABLE_GNT
GNT_DIR=finch
endif

SUBDIRS = libpurple doc $(GNT_DIR) $(GTK_DIR) m4macros po share

docs: Doxyfile
if HAVE_DOXYGEN
	@echo "Running doxygen..."
	@doxygen
else
	@echo "doxygen was not found during configure.  Aborting."
	@echo;
endif

# perl's MakeMaker uninstall foo doesn't work well with DESTDIR set, which
# breaks "make distcheck" unless we ignore perl things

distuninstallcheck_listfiles = \
	find . -type f -print | grep -v perl | grep -v Purple.3pm

DISTCLEANFILES= pidgin.desktop libpurple/gconf/purple.schemas intltool-extract \
			intltool-merge intltool-update
