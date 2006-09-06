project(owqtutil)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

# needed include directories to build owqtutil
# saves the variable in internal cache for later use
set(OWQTUTIL_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWTHREAD_INCLUDE_DIRS}
  ${QT_INCLUDE_DIR}
  CACHE INTERNAL "owqtutil include directories"
)

# owqtutil lib and dependencies
# saves the variable in internal cache for later use
set(OWQTUTIL_LIBRARIES
  owqtutil
  ${OWTHREAD_LIBRARIES}
  CACHE INTERNAL "owqtutil libraries"
)

include_directories(
  ${OWQTUTIL_INCLUDE_DIRS}
)

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
  src/SafeConnect.cpp
)

qt4_automoc(${owqtutil_SRCS})
qt4_wrap_ui(owqtutil_SRCS src/Wizard.ui)

add_library(owqtutil STATIC ${owqtutil_SRCS})

target_link_libraries(${OWQTUTIL_LIBRARIES})
