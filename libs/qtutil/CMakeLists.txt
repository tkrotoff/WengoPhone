project(owqtutil)

#Qt4 dependency
find_package(Qt4 REQUIRED)
include_directories(
	${QT_INCLUDE_DIR}
)
include(${QT_USE_FILE})

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
	src/WengoStyleLabel.cpp
	src/ToolTipLineEdit.cpp
	src/ClickableLabel.cpp
	src/WidgetBackgroundImage.cpp
	src/Wizard.cpp
	src/ThreadEventFilter.cpp
)

qt4_automoc(${owqtutil_SRCS})

include_directories(${owqtutil_BINARY_DIR})
qt4_wrap_ui(owqtutil_SRCS src/Wizard.ui)

#owqtutil
add_library(owqtutil STATIC ${owqtutil_SRCS})

#owutil dependency
target_link_libraries(owqtutil owutil)
