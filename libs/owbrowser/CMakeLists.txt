project(owbrowser)

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

#owqtutil dependency
include_directories(${owqtutil_INCLUDE_DIR})

#includes
include_directories(${owbrowser_SOURCE_DIR})

#sources
set(owbrowser_SRCS
	QtBrowser.cpp
)

qt4_automoc(${owbrowser_SRCS})

#owidle
add_library(owbrowser STATIC ${owbrowser_SRCS})

#owqtutil dependency
target_link_libraries(owbrowser owqtutil)
