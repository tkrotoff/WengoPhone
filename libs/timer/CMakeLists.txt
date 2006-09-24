project(wtimer)

# TODO: test

# needed include directories to build wtimer
# saves the variable in internal cache for later use
set(WTIMER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  CACHE INTERNAL "wtimer include directories"
)

set(WTIMER_LIBRARY
  wtimer
  CACHE INTERNAL "wtimer library"
)

# wtimer lib and dependencies
set(WTIMER_LINK_LIBRARIES
  ${WTIMER_LIBRARY}
)

set(wtimer_SRCS
  src/timer_impl.c
)

if (WIN32)
  set(wtimer_SRCS
    ${wtimer_SRCS}
    src/win32_mm/impl_timer.c
  )

  add_definitions(-DTIMER_ENABLE_WINMM_IMPL=1)

  set(WTIMER_LINK_LIBRARIES
    ${WTIMER_LINK_LIBRARIES}
    winmm
  )
else (WIN32)
  if (UNIX)
    set(WTIMER_LINK_LIBRARIES
      ${WTIMER_LINK_LIBRARIES}
      pthread
      rt
    )
  endif (UNIX)

  add_definitions(-DTIMER_ENABLE_CLOCK_GETTIME_IMPL=1)

  set(wtimer_SRCS
    ${wtimer_SRCS}
    src/clock_gettime/impl_timer.c
  )

endif (WIN32)

include_directories(
  ${WTIMER_INCLUDE_DIRS}
)

add_library(${WTIMER_LIBRARY} STATIC ${wtimer_SRCS})

target_link_libraries(${WTIMER_LINK_LIBRARIES})

