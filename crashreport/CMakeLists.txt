project(owcrashreport)

# needed include directories to build owcrashreport
# saves the variable in internal cache for later use
set(OWCRASHREPORT_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWCUTIL_INCLUDE_DIRS}
  ${OWUTIL_INCLUDE_DIRS}
  ${OWTHREAD_INCLUDE_DIRS}
  ${OWCURL_INCLUDE_DIRS}
  ${OWQTUTIL_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
  CACHE INTERNAL "owcrashreport include directories"
)

set(OWCRASHREPORT_EXECUTABLE
  owcrashreport
  CACHE INTERNAL "owcrashreport library"
)

# owcrashreport lib and dependencies
set(OWCRASHREPORT_LINK_LIBRARIES
  ${OWCRASHREPORT_EXECUTABLE}
  ${OWCUTIL_LIBRARY}
  ${OWUTIL_LIBRARY}
  ${OWTHREAD_LIBRARY}
  ${OWCURL_LIBRARY}
  ${OWQTUTIL_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${BOOST_PROGRAM_OPTIONS_LIBRARY}
)

qt4_wrap_cpp(owcrashreport_SRCS
  FtpUpload.h
  QtCrashReport.h
)

set(owcrashreport_SRCS
  ${owcrashreport_SRCS}
  main.cpp
  FtpUpload.cpp
  QtCrashReport.cpp
)

qt4_wrap_ui(owcrashreport_SRCS
  CrashReport.ui
)

include_directories(
  ${OWCRASHREPORT_INCLUDE_DIRS}
)

add_executable(${OWCRASHREPORT_EXECUTABLE} ${owcrashreport_SRCS})

target_link_libraries(${OWCRASHREPORT_LINK_LIBRARIES})

install(
  TARGETS
    ${OWCRASHREPORT_EXECUTABLE}
  DESTINATION
    ${BIN_INSTALL_DIR}
)

