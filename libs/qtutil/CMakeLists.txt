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
  ${QT_QTUITOOLS_LIBRARY}
)

qt4_wrap_cpp(owqtutil_SRCS
  include/qtutil/EventFilter.h
  include/qtutil/KeyEventFilter.h
  include/qtutil/MouseEventFilter.h
  include/qtutil/PaintEventFilter.h
  include/qtutil/EnabledChangeEventFilter.h
  include/qtutil/PostEvent.h
  include/qtutil/WidgetFactory.h
  include/qtutil/Object.h
  include/qtutil/QObjectThreadSafe.h
  include/qtutil/ResizeEventFilter.h
  include/qtutil/NonResizeableWidget.h
  include/qtutil/StringListConvert.h
  include/qtutil/Widget.h
  include/qtutil/WengoStyleLabel.h
  include/qtutil/ToolTipLineEdit.h
  include/qtutil/ClickableLabel.h
  include/qtutil/WidgetBackgroundImage.h
  include/qtutil/Wizard.h
  include/qtutil/QtThreadEvent.h
  include/qtutil/ThreadEventFilter.h
  include/qtutil/PixmapMerging.h
  include/qtutil/SafeConnect.h
  include/qtutil/LanguageChangeEventFilter.h
  include/qtutil/CloseEventFilter.h
)

set(owqtutil_SRCS
  ${owqtutil_SRCS}
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

qt4_wrap_ui(owqtutil_SRCS src/Wizard.ui)

include_directories(
  ${OWQTUTIL_INCLUDE_DIRS}
)

add_library(${OWQTUTIL_LIBRARY} STATIC ${owqtutil_SRCS})

target_link_libraries(${OWQTUTIL_LINK_LIBRARIES})

