project(owmemorydump)

# needed include directories to build owmemorydump
# saves the variable in internal cache for later use
set(OWMEMORYDUMP_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  CACHE INTERNAL "owmemorydump include directories"
)

# owmemorydump lib and dependencies
# saves the variable in internal cache for later use
set(OWMEMORYDUMP_LIBRARIES
  owmemorydump
  ${OWUTIL_LIBRARIES}
  CACHE INTERNAL "owmemorydump libraries"
)

if (MSVC)
  set(OWMEMORYDUMP_LIBRARIES
    ${set(OWMEMORYDUMP_LIBRARIES}
    shlwapi
    CACHE INTERNAL "owmemorydump libraries"
  )
endif (MSVC)

include_directories(
  ${OWMEMORYDUMP_INCLUDE_DIRS}
)

set(owmemorydump_SRCS
  src/win32/MemoryDump.cpp
)

add_library(owmemorydump STATIC ${owmemorydump_SRCS})

target_link_libraries(${OWMEMORYDUMP_LIBRARIES})

