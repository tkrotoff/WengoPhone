project(owqtutil)

include(${QT_USE_FILE})

# needed include directories to build owqtutil
# saves the variable in internal cache for later use
set(OWQTUTIL_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  ${OWTHREAD_INCLUDE_DIRS}
  ${QT_INCLUDE_DIR}
  CACHE INTERNAL "owqtutil include directories"
)

set(OWQTUTIL_LIBRARY
  owqtutil
  CACHE INTERNAL "owqtutil library"
)

# owqtutil lib and dependencies
set(OWQTUTIL_LINK_LIBRARIES
  ${OWQTUTIL_LIBRARY}
  ${OWUTIL_LIBRARY}
  ${OWTHREAD_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
)

set(owqtutil_SRCS
  src/CloseEventFilter.cpp
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
  src/SafeConnect.cpp
)

qt4_automoc(${owqtutil_SRCS})
qt4_wrap_ui(owqtutil_SRCS src/Wizard.ui)


include_directories(
  ${OWQTUTIL_INCLUDE_DIRS}
)

add_library(${OWQTUTIL_LIBRARY} STATIC ${owqtutil_SRCS})

target_link_libraries(${OWQTUTIL_LINK_LIBRARIES})

