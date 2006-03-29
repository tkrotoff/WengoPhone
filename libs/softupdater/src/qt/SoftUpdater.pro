TEMPLATE = app
LANGUAGE = C++

CONFIG += qt stl warn_on release

TARGET = softupdater

HEADERS += SoftUpdater.h

SOURCES += main.cpp \
	SoftUpdater.cpp

UI_DIR = tmp/ui
MOC_DIR = tmp/moc
OBJECTS_DIR = tmp/obj

unix {
	QMAKE_CXXFLAGS += -Wall -ansi
	QMAKE_CFLAGS += -Wall -ansi
}
