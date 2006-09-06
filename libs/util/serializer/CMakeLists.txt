project(owserializer)

# needed include directories to build owserializer
# saves the variable in internal cache for later use
set(OWSERIALIZER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWSERIALIZATION_INCLUDE_DIRS}
  ${TINYXML_INCLUDE_DIRS}
  CACHE INTERNAL "owserializer include directories"
)

# owserializer lib and dependencies
# saves the variable in internal cache for later use
set(OWSERIALIZER_LIBRARIES
  owserializer
  ${OWSERIALIZATION_LIBRARIES}
  ${TINYXML_LIBRARIES}
  CACHE INTERNAL "owserializer libraries"
)

include_directories(
  ${OWSERIALIZER_INCLUDE_DIRS}
)

#sources
set(owserializer_SRCS
  src/DateXMLSerializer.cpp
  src/TimeXMLSerializer.cpp
)

#owserializer
add_library(owserializer STATIC ${owserializer_SRCS})

target_link_libraries(${OWSERIALIZER_LIBRARIES})

