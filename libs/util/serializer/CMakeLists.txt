project(owserializer)

# needed include directories to build owserializer
# saves the variable in internal cache for later use
set(OWSERIALIZER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWSERIALIZATION_INCLUDE_DIRS}
  ${TINYXML_INCLUDE_DIRS}
  CACHE INTERNAL "owserializer include directories"
)

set(OWSERIALIZER_LIBRARY
  owserializer
  CACHE INTERNAL "owserializer library"
)

# owserializer lib and dependencies
set(OWSERIALIZER_LINK_LIBRARIES
  ${OWSERIALIZER_LIBRARY}
  ${OWSERIALIZATION_LIBRARY}
  ${TINYXML_LIBRARIES}
)

set(owserializer_SRCS
  src/DateXMLSerializer.cpp
  src/TimeXMLSerializer.cpp
)


include_directories(
  ${OWSERIALIZER_INCLUDE_DIRS}
)

add_library(${OWSERIALIZER_LIBRARY} STATIC ${owserializer_SRCS})

target_link_libraries(${OWSERIALIZER_LINK_LIBRARIES})

