project(owtrayicon)

#Qt4 dependency
find_package(Qt4 REQUIRED)
include_directories(
	${QT_INCLUDE_DIR}
	${QT_QTNETWORK_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTGUI_INCLUDE_DIR}
	${QT_QTXML_INCLUDE_DIR}
	${QT_QTSVG_INCLUDE_DIR}
	${QT_INCLUDE_DIR}/QtUiTools
)

#includes
include_directories(${owtrayicon_SOURCE_DIR})

#sources
set(owtrayicon_SRCS
	trayicon.cpp
)

if (WIN32)
	set(owtrayicon_SRCS ${owtrayicon_SRCS}
		trayicon_win.cpp
	)
endif (WIN32)

if (APPLE)
	set(owtrayicon_SRCS ${owtrayicon_SRCS}
		trayicon_mac.cpp
	)
endif (APPLE)

if (UNIX)
	set(owtrayicon_SRCS ${owtrayicon_SRCS}
		trayicon_x11.cpp
	)
endif (UNIX)

qt4_automoc(${owtrayicon_SRCS})

#owtrayicon
add_library(owtrayicon STATIC ${owtrayicon_SRCS})
