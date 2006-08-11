project(owqtutil)

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

#owthread dependency
include_directories(${owthread_INCLUDE_DIR})

#includes
set(owqtutil_INCLUDE_DIR
	${owqtutil_SOURCE_DIR}/include
	${owutil_INCLUDE_DIR}
	CACHE INTERNAL ""
)

include_directories(${owqtutil_INCLUDE_DIR})

#sources
set(owqtutil_SRCS
	src/EventFilter.cpp
	src/KeyEventFilter.cpp
	src/MouseEventFilter.cpp
	src/PaintEventFilter.cpp
	src/EnabledChangeEventFilter.cpp
	src/WidgetFactory.cpp
	src/Object.cpp
	src/QObjectThreadSafe.cpp
	src/ResizeEventFilter.cpp
	src/NonResizeableWidget.cpp
	src/StringListConvert.cpp
	src/Widget.cpp
	src/QtWengoStyleLabel.cpp
	src/ToolTipLineEdit.cpp
	src/QtClickableLabel.cpp
	src/WidgetBackgroundImage.cpp
	src/Wizard.cpp
	src/ThreadEventFilter.cpp
)

qt4_automoc(${owqtutil_SRCS})

#owqtutil
add_library(owqtutil STATIC ${owqtutil_SRCS})

#owutil dependency
target_link_libraries(owqtutil owutil)
