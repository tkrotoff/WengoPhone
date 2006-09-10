project(wtimer)

# needed include directories to build wtimer
# saves the variable in internal cache for later use
set(WTIMER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  CACHE INTERNAL "wtimer include directories"
)

# wtimer lib and dependencies
# saves the variable in internal cache for later use
set(WTIMER_LIBRARIES
  wtimer
  CACHE INTERNAL "wtimer libraries"
)

include_directories(
  ${WTIMER_INCLUDE_DIRS}
)

if (WIN32)
  set(wtimer_SRCS
    src/win32_mm/impl_timer.c
  )

  add_definitions(-DTIMER_ENABLE_WINMM_IMPL)
else (WIN32)
  if (UNIX AND NOT WIN32)
    set(WTIMER_LIBRARIES
      ${WTIMER_LIBRARIES}
      pthread
      rt
    )
  endif (UNIX AND NOT WIN32)

  set(wtimer_SRCS
    src/clock_gettime/impl_timer.c
  )

  add_definitions(-DTIMER_ENABLE_CLOCK_GETTIME_IMPL)
endif (WIN32)

add_library(wtimer STATIC ${wtimer_SRCS})

target_link_libraries(${WTIMER_LIBRARIES})

