INCLUDES = \
	-I$(srcdir)/include \
	-I$(srcdir)/include/generated \
	$(all_includes)

lib_LTLIBRARIES = libvcard.la

libvcard_la_SOURCES = vCard-all.cpp

check_PROGRAMS = testwrite testread

testwrite_LDFLAGS = $(all_libraries) $(KDE_RPATH)
testwrite_LDADD = libvcard.la $(LIB_KDECORE)
testwrite_SOURCES = testwrite.cpp

testread_LDFLAGS = $(all_libraries) $(KDE_RPATH)
testread_LDADD = libvcard.la $(LIB_KDECORE)
testread_SOURCES = testread.cpp
