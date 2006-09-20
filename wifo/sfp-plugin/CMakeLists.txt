project(sfp-plugin)

# needed include directories to build sfp-plugin
# saves the variable in internal cache for later use
set(SFP-PLUGIN_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${PHAPI_INCLUDE_DIRS}
  ${PHAPI-UTIL_INCLUDE_DIRS}
  ${EXOSIP_INCLUDE_DIRS}
  CACHE INTERNAL "sfp-plugin include directories"
)

set(SFP-PLUGIN_LIBRARY
  sfp-plugin
  CACHE INTERNAL "sfp-plugin library"
)

# sfp-plugin lib and dependencies
set(SFP-PLUGIN_LINK_LIBRARIES
  ${SFP-PLUGIN_LIBRARY}
  ${PHAPI-UTIL_LIBRARY}
  ${PHAPI_LIBRARY}
  ${EXOSIP_LIBRARY}
)

if (MSVC)
  set(SFP-PLUGIN_DEFINITIONS
    -DBUILD_SFP_PLUGIN_DLL
    -DWIN32
  )
endif (MSVC)

set(sfp-plugin_SRCS
  src/sfp-parser.c
  src/sfp-plugin.c
  src/sfp-transfer.c
)

include_directories(
  ${SFP-PLUGIN_INCLUDE_DIRS}
)

add_definitions(
  ${SFP-PLUGIN_DEFINITIONS}
)

if (APPLE)
  add_library(${SFP-PLUGIN_LIBRARY} STATIC ${sfp-plugin_SRCS})
  install(
    TARGETS
      ${SFP-PLUGIN_LIBRARY}
    ARCHIVE DESTINATION
      ${LIB_INSTALL_DIR}
  )
else (APPLE)
  add_library(${SFP-PLUGIN_LIBRARY} SHARED ${sfp-plugin_SRCS})
  install(
    TARGETS
      ${SFP-PLUGIN_LIBRARY}
    LIBRARY DESTINATION
      ${LIB_INSTALL_DIR}
  )
endif (APPLE)

target_link_libraries(${SFP-PLUGIN_LINK_LIBRARIES})

