TEMPLATE = lib

CONFIG += debug console staticlib

TARGET = bin/zip

HEADERS += \
    zip.h

SOURCES += \
    zip.cpp \
    minizip/unzip.c

UI_DIR = tmp/ui
MOC_DIR = tmp/moc
OBJECTS_DIR = tmp/obj

win32: {
    INCLUDEPATH += minizip/win32
    LIBS += minizip/win32/libz.a
}
mac: {
    QMAKE_LFLAGS += -lz
}
