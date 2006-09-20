project(owgaimwrapper)

# needed include directories to build owgaimwrapper
# saves the variable in internal cache for later use
set(OWGAIMWRAPPER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OWCUTIL_INCLUDE_DIRS}
  ${OWIMWRAPPER_INCLUDE_DIRS}
  ${GAIM_INCLUDE_DIRS}
  ${GTK2_INCLUDE_DIRS}
  CACHE INTERNAL "owgaimwrapper include directories"
)

set(OWGAIMWRAPPER_LIBRARY
  owgaimwrapper
  CACHE INTERNAL "owgaimwrapper library"
)

# owgaimwrapper lib and dependencies
# saves the variable in internal cache for later use
set(OWGAIMWRAPPER_LIBRARIES
  owgaimwrapper
  ${OWCUTIL_LIBRARIES}
  ${OWIMWRAPPER_LIBRARIES}
  ${GAIM_LIBRARIES}
  ${GTK2_GLIB_LIBRARY}
  ${GTK2_GTHREAD_LIBRARY}
  CACHE INTERNAL "owgaimwrapper libraries"
)

if (UNIX)
  set(OWGAIMWRAPPER_LIBRARIES
    ${OWGAIMWRAPPER_LIBRARIES}
    ${GTK2_GTHREAD_LIBRARIES}
  )

  add_definitions(-DENABLE_NLS)
endif (UNIX)

set(owgaimwrapper_SRCS
  GaimIMFactory.cpp
  GaimIMChat.cpp
  GaimIMConnect.cpp
  GaimIMPresence.cpp
  GaimIMContactList.cpp
  GaimChatMngr.cpp
  GaimConnectMngr.cpp
  GaimPresenceMngr.cpp
  GaimAccountMngr.cpp
  GaimContactListMngr.cpp
  GaimEnumPresenceState.cpp
  GaimEnumIMProtocol.cpp
)

include_directories(
  ${OWGAIMWRAPPER_INCLUDE_DIRS}
)


add_library(owgaimwrapper STATIC ${owgaimwrapper_SRCS})

target_link_libraries(${OWGAIMWRAPPER_LIBRARIES})

