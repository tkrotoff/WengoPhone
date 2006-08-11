project(owidle)

#Qt4 dependency
find_package(Qt4 REQUIRED)
include_directories(
	${QT_INCLUDE_DIR}
)

#owqtutil dependency
include_directories(${owqtutil_INCLUDE_DIR})

#includes
set(owidle_INCLUDE_DIR
	${owidle_SOURCE_DIR}/include
	CACHE INTERNAL ""
)

include_directories(${owidle_INCLUDE_DIR})

#sources
set(owidle_SRCS
	src/Idle.cpp
)

qt4_automoc(${owidle_SRCS})

#owidle
add_library(owidle STATIC ${owidle_SRCS})

#owqtutil dependency
target_link_libraries(owidle owqtutil)
