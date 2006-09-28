project(owsipwrapper)

#subdirs(
#  src/phapi
#)

# needed include directories to build owsipwrapper
# saves the variable in internal cache for later use
set(OWSIPWRAPPER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  CACHE INTERNAL "owsipwrapper include directories"
)

set(OWSIPWRAPPER_LIBRARY
  owsipwrapper
  CACHE INTERNAL "owsipwrapper library"
)

# owsipwrapper lib and dependencies
set(OWSIPWRAPPER_LINK_LIBRARIES
  ${OWSIPWRAPPER_LIBRARY}
  ${OWUTIL_LIBRARY}
)

set(owsipwrapper_SRCS
  src/SipWrapperFactory.cpp
  src/EnumPhoneCallState.cpp
  src/EnumPhoneLineState.cpp
  src/EnumNatType.cpp
  src/EnumVideoQuality.cpp
  src/CodecList.cpp
)

include_directories(
  ${OWSIPWRAPPER_INCLUDE_DIRS}
)


add_library(${OWSIPWRAPPER_LIBRARY} STATIC ${owsipwrapper_SRCS})

target_link_libraries(${OWSIPWRAPPER_LINK_LIBRARIES})

