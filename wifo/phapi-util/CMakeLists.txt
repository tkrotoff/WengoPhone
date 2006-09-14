project(phapi-util)

# needed include directories to build phapi-util
# saves the variable in internal cache for later use
set(PHAPI-UTIL_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/phapi-util/msvc
  CACHE INTERNAL "phapi-util include directories"
)

# phapi-util lib and dependencies
# saves the variable in internal cache for later use
set(PHAPI-UTIL_LIBRARIES
  phapi-util
  CACHE INTERNAL "phapi-util libraries"
)

include_directories(
  ${PHAPI-UTIL_INCLUDE_DIRS}
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

add_library(phapi-util STATIC ${phapi-util_SRCS})

target_link_libraries(${PHAPI-UTIL_LIBRARIES})

