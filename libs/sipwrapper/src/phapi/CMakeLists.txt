project(owphapiwrapper)

# needed include directories to build owphapiwrapper
# saves the variable in internal cache for later use
set(OWPHAPIWRAPPER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OWTHREAD_INCLUDE_DIRS}
  ${OWUTIL_INCLUDE_DIRS}
  ${OWSOUND_INCLUDE_DIRS}
  ${TINYXML_INCLUDE_DIRS}
  ${PIXERTOOL_INCLUDE_DIRS}
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
set(OWPHAPIWRAPPER_LINK_LIBRARIES
  ${OWPHAPIWRAPPER_LIBRARY}
  ${OWUTIL_LIBRARY}
  ${OWTHREAD_LIBRARY}
  ${OWSOUND_LIBRARY}
  ${TINYXML_LIBRARY}
  ${PIXERTOOL_LIBRARY}
  ${OWIMWRAPPER_LIBRARY}
  ${OWSIPWRAPPER_LIBRARY}
  ${PHAPI_LIBRARY}
  ${ORTP_LIBRARY}
  ${STUN_LIBRARY}
  ${HTTPTUNNEL_LIBRARY}
  ${SFP-PLUGIN_LIBRARY}
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
  set(OWPHAPIWRAPPER_LINK_LIBRARIES
    ${OWPHAPIWRAPPER_LINK_LIBRARIES}
    ${PIXERTOOL_LIBRARY}
  )

  add_definitions(-DENABLE_VIDEO)
endif (WITH_VIDEO)

include_directories(
  ${OWPHAPIWRAPPER_INCLUDE_DIRS}
)


add_library(${OWPHAPIWRAPPER_LIBRARY} STATIC ${owphapiwrapper_SRCS})

if (APPLE)
  macro_add_link_flags(${OWPHAPIWRAPPER_LIBRARY}
    "-framework CoreFoundation"
  )
endif (APPLE)

target_link_libraries(${OWPHAPIWRAPPER_LINK_LIBRARIES})

