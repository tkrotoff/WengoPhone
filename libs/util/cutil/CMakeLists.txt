project(owcutil)

set(OWCUTIL_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  CACHE INTERNAL "owcutil include directory"
)

if (MSVC)
  set(OWCUTIL_INCLUDE_DIRS
    ${owcutil_SOURCE_DIR}/include/cutil/msvc
    CACHE INTERNAL "owcutil include directory"
  )
endif (MSVC)

set(OWCUTIL_LIBRARIES
  owcutil
  CACHE INTERNAL "owcutil libraries"
)

include_directories(${OWCUTIL_INCLUDE_DIRS})

set(owcutil_SRCS
  src/fake.c
  src/strlcat.c
  src/strlcpy.c
)

if (MSVC)
  set(owcutil_SRCS
    ${owcutil_SRCS}
    src/msvc/dirent.c
  )
endif (MSVC)

add_library(owcutil STATIC ${owcutil_SRCS})

