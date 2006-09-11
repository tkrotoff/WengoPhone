project(owhttp)

# needed include directories to build owhttp
# saves the variable in internal cache for later use
set(OWHTTP_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWTHREAD_INCLUDE_DIRS}
  CACHE INTERNAL "owhttp include directories"
)

# owhttp lib and dependencies
# saves the variable in internal cache for later use
set(OWHTTP_LIBRARIES
  owhttp
  ${OWTHREAD_LIBRARIES}
  CACHE INTERNAL "owhttp libraries"
)

if (WIN32)
  set(OWHTTP_LIBRARIES
    ${OWHTTP_LIBRARIES}
    advapi32
    shell32
    CACHE INTERNAL "owhttp libraries"
  )
endif (WIN32)

include_directories(
  ${OWHTTP_INCLUDE_DIRS}
)

#sources
set(owhttp_SRCS
  src/HttpRequest.cpp
  src/null/NullHttpRequest.cpp
  src/curl/CurlHttpRequest.cpp
  #src/qt/QtHttpRequest.cpp
)

add_library(owhttp STATIC ${owhttp_SRCS})

target_link_libraries(${OWTHREAD_LIBRARIES})

if (APPLE)
  set_target_properties(
    PROPERTIES
      LINK_FLAGS
        -framework CoreFoundation
  )
endif (APPLE)

