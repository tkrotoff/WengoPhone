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

# sfp-plugin lib and dependencies
# saves the variable in internal cache for later use
set(SFP-PLUGIN_LIBRARIES
  sfp-plugin
  ${PHAPI_LIBRARY}
  ${PHAPI-UTIL_LIBRARIES}
  ${EXOSIP_LIBRARIES}
  CACHE INTERNAL "sfp-plugin libraries"
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
  add_library(sfp-plugin STATIC ${sfp-plugin_SRCS})
  install(
    TARGETS
      sfp-plugin
    ARCHIVE DESTINATION
      ${LIB_INSTALL_DIR}
  )
else (APPLE)
  add_library(sfp-plugin SHARED ${sfp-plugin_SRCS})
  install(
    TARGETS
      sfp-plugin
    LIBRARY DESTINATION
      ${LIB_INSTALL_DIR}
  )
endif (APPLE)

target_link_libraries(${SFP-PLUGIN_LIBRARIES})


