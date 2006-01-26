TEMPLATE = lib
LANGUAGE = C++

CONFIG += staticlib qt stl warn_on console debug
QT += network

TARGET = util
DESTDIR = bin

INCLUDEPATH += include
INCLUDEPATH += ../../libs/curl/include

HEADERS += include/http/QtHttpRequest.h

SOURCES += src/Startup.cpp \
	src/WebBrowser.cpp \
	src/StringList.cpp \
	src/http/HttpRequest.cpp \
	src/http/CurlHttpRequest.cpp \
	src/http/NullHttpRequest.cpp \
	src/http/QtHttpRequest.cpp

win32:msvc {
	SOURCES += src/win32/MemoryDump.cpp
}

UI_DIR = bin/ui
MOC_DIR = bin/moc
OBJECTS_DIR = bin/obj

unix {
	QMAKE_CXXFLAGS += -Wall -ansi
	QMAKE_CFLAGS += -Wall -ansi

	LIBS += 
}

win32 {
	LIBS += 
}
