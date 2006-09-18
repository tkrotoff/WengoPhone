project(owphapiwrapper)

# needed include directories to build owphapiwrapper
# saves the variable in internal cache for later use
set(OWPHAPIWRAPPER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OWTHREAD_INCLUDE_DIRS}
  ${OWUTIL_INCLUDE_DIRS}
  ${OWSOUND_INCLUDE_DIRS}
  ${TINYXML_INCLUDE_DIRS}
  ${OWIMWRAPPER_INCLUDE_DIRS}
  ${OWSIPWRAPPER_INCLUDE_DIRS}
  ${PHAPI_INCLUDE_DIRS}
  ${SFP-PLUGIN_INCLUDE_DIRS}
  CACHE INTERNAL "owphapiwrapper include directories"
)

set(OWPHAPIWRAPPER_LIBRARY
  owphapiwrapper
  CACHE INTERNAL "owphapiwrapper library"
)

# owphapiwrapper lib and dependencies
# saves the variable in internal cache for later use
set(OWPHAPIWRAPPER_LIBRARIES
  owphapiwrapper
  ${OWTHREAD_LIBRARIES}
  ${OWUTIL_LIBRARIES}
  ${OWSOUND_LIBRARIES}
  ${TINYXML_LIBRARIES}
  ${OWIMWRAPPER_LIBRARIES}
  ${OWSIPWRAPPER_LIBRARIES}
  ${PHAPI_LIBRARY}
  ${ORTP_LIBRARIES}
  ${STUN_LIBRARIES}
  ${HTTPTUNNEL_LIBRARIES}
  ${SFP-PLUGIN_LIBRARIES}
  CACHE INTERNAL "owphapiwrapper libraries"
)

set(owphapiwrapper_SRCS
  PhApiFactory.cpp
  PhApiWrapper.cpp
  PhApiCallbacks.cpp
  PhApiIMChat.cpp
  PhApiIMConnect.cpp
  PhApiIMPresence.cpp
  PhApiCodecList.cpp
  PhApiSFPCallbacks.cpp
  PhApiSFPWrapper.cpp
  PhApiSFPEvent.cpp
)

if (WITH_VIDEO)
  set(OWPHAPIWRAPPER_LIBRARIES
    ${OWPHAPIWRAPPER_LIBRARIES}
    ${PIXERTOOL_LIBRARIES}
    CACHE INTERNAL "owphapiwrapper libraries"
  )

  add_definitions(-DENABLE_VIDEO)
endif (WITH_VIDEO)

include_directories(
  ${OWPHAPIWRAPPER_INCLUDE_DIRS}
)


add_library(owphapiwrapper STATIC ${owphapiwrapper_SRCS})

if (APPLE)
  macro_add_link_flags(owphapiwrapper
    "-framework CoreFoundation"
  )
endif (APPLE)

target_link_libraries(${OWPHAPIWRAPPER_LIBRARIES})

