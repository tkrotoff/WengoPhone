project(owcutil)

set(OWCUTIL_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  CACHE INTERNAL "owcutil include directory"
)

set(OWCUTIL_LIBRARY
  owcutil
  CACHE INTERNAL "owcutil library"
)

set(OWCUTIL_LINK_LIBRARIES
  ${OWCUTIL_LIBRARY}
)

set(owcutil_SRCS
  src/fake.c
  src/strlcat.c
  src/strlcpy.c
)

if (MSVC)
  set(OWCUTIL_INCLUDE_DIRS
    ${OWCUTIL_INCLUDE_DIRS}
    ${owcutil_SOURCE_DIR}/include/cutil/msvc
    CACHE INTERNAL "owcutil include directory"
  )
endif (MSVC)

include_directories(${OWCUTIL_INCLUDE_DIRS})

if (MSVC)
  set(owcutil_SRCS
    ${owcutil_SRCS}
    src/msvc/dirent.c
  )
endif (MSVC)

add_library(${OWCUTIL_LIBRARY} STATIC ${owcutil_SRCS})

