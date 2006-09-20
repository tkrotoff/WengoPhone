project(phapi-util)

# needed include directories to build phapi-util
# saves the variable in internal cache for later use
set(PHAPI-UTIL_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  CACHE INTERNAL "phapi-util include directories"
)

if (WIN32)
  set(PHAPI-UTIL_INCLUDE_DIRS
    ${PHAPI-UTIL_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/phapi-util/msvc
    CACHE INTERNAL "phapi-util include directories"
  )
endif (WIN32)

set(PHAPI-UTIL_LIBRARY
  phapi-util
  CACHE INTERNAL "phapi-util library"
)

# phapi-util lib and dependencies
set(PHAPI-UTIL_LINK_LIBRARIES
  ${PHAPI-UTIL_LIBRARY}
)

set(phapi-util_SRCS
  src/util.c
  src/comparator.c
  src/linkedlist.c
  src/mappinglist.c
)

if (WIN32)
  add_definitions(
    -DWIN32
  )

  set(phapi-util_SRCS
    ${phapi-util_SRCS}
    src/msvc/pthread.c
  )
endif (WIN32)

include_directories(
  ${PHAPI-UTIL_INCLUDE_DIRS}
)

add_library(${PHAPI-UTIL_LIBRARY} STATIC ${phapi-util_SRCS})

target_link_libraries(${PHAPI-UTIL_LINK_LIBRARIES})

