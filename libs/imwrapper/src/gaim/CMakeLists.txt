project(owgaimwrapper)

# needed include directories to build owgaimwrapper
# saves the variable in internal cache for later use
set(OWGAIMWRAPPER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OWCUTIL_INCLUDE_DIRS}
  ${OWIMWRAPPER_INCLUDE_DIRS}
  ${GAIM_INCLUDE_DIRS}
  ${GLIB2_INCLUDE_DIRS}
  CACHE INTERNAL "owgaimwrapper include directories"
)

set(OWGAIMWRAPPER_LIBRARY
  owgaimwrapper
  CACHE INTERNAL "owgaimwrapper library"
)

# owgaimwrapper lib and dependencies
set(OWGAIMWRAPPER_LINK_LIBRARIES
  ${OWGAIMWRAPPER_LIBRARY}
  ${OWCUTIL_LIBRARY}
  ${OWIMWRAPPER_LIBRARY}
  ${GAIM_LIBRARY}
)

if (UNIX)
  set(OWGAIMWRAPPER_LINK_LIBRARIES
    ${OWGAIMWRAPPER_LINK_LIBRARIES}
    ${GTHREAD2_LIBRARIES}
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


add_library(${OWGAIMWRAPPER_LIBRARY} STATIC ${owgaimwrapper_SRCS})

target_link_libraries(${OWGAIMWRAPPER_LINK_LIBRARIES})

