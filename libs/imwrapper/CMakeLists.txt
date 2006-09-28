project(owimwrapper)

# TODO: test
#subdirs(
#  src/gaim
#  src/multiim
#  #src/tests
#)

if (WIN32)
  find_package(GAIMInternal REQUIRED)
endif (WIN32)

# needed include directories to build owimwrapper
# saves the variable in internal cache for later use
set(OWIMWRAPPER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWSETTINGS_INCLUDE_DIRS}
  ${TINYXML_INCLUDE_DIRS}
  ${GLIB2_INCLUDE_DIR}
  CACHE INTERNAL "owimwrapper include directories"
)

set(OWIMWRAPPER_LIBRARY
  owimwrapper
  CACHE INTERNAL "owimwrapper library"
)

# owimwrapper lib and dependencies
set(OWIMWRAPPER_LINK_LIBRARIES
  ${OWIMWRAPPER_LIBRARY}
  ${OWSETTINGS_LIBRARY}
  ${TINYXML_LIBRARY}
  ${GLIB2_LIBRARY}
  CACHE INTERNAL "owimwrapper libraries"
)

set(owimwrapper_SRCS
  src/Account.cpp
  src/EnumIMProtocol.cpp
  src/EnumPresenceState.cpp
  src/IMAccount.cpp
  src/IMAccountHandler.cpp
  src/IMAccountHandlerFileStorage.cpp
  src/IMAccountHandlerXMLSerializer.cpp
  src/IMAccountParameters.cpp
  src/IMAccountParametersXMLSerializer.cpp
  src/IMAccountXMLSerializer.cpp
  src/IMChatSession.cpp
  src/IMContact.cpp
  src/IMContactSet.cpp
  src/IMContactXMLSerializer.cpp
  src/IMWrapperFactory.cpp
)

include_directories(
  ${OWIMWRAPPER_INCLUDE_DIRS}
)

add_library(${OWIMWRAPPER_LIBRARY} STATIC ${owimwrapper_SRCS})

target_link_libraries(${OWIMWRAPPER_LINK_LIBRARIES})

