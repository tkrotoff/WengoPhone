TEMPLATE = lib

CONFIG += qt debug console staticlib

TARGET = bin/iconset

INCLUDEPATH += ../zip psipng

SOURCES += iconset.cpp anim.cpp
HEADERS += iconset.h anim.h

win32{
	LIBS += ../zip/bin/zip.lib
}
unix{
	LIBS += ../zip/bin/libzip.a
}

UI_DIR = tmp/ui
MOC_DIR = tmp/moc
OBJECTS_DIR = tmp/obj
