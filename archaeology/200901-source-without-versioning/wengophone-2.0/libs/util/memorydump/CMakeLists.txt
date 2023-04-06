project(owmemorydump)

# needed include directories to build owmemorydump
# saves the variable in internal cache for later use
set(OWMEMORYDUMP_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  CACHE INTERNAL "owmemorydump include directories"
)

set(OWMEMORYDUMP_LIBRARY
  owmemorydump
  CACHE INTERNAL "owmemorydump library"
)

# owmemorydump lib and dependencies
set(OWMEMORYDUMP_LINK_LIBRARIES
  ${OWMEMORYDUMP_LIBRARY}
  ${OWUTIL_LIBRARY}
)

if (MSVC)
  set(OWMEMORYDUMP_LINK_LIBRARIES
    ${OWMEMORYDUMP_LINK_LIBRARIES}
    shlwapi
  )
endif (MSVC)

set(owmemorydump_SRCS
  src/win32/MemoryDump.cpp
)

include_directories(
  ${OWMEMORYDUMP_INCLUDE_DIRS}
)

add_library(${OWMEMORYDUMP_LIBRARY} STATIC ${owmemorydump_SRCS})

target_link_libraries(${OWMEMORYDUMP_LINK_LIBRARIES})

