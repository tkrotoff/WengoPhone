project(ownullwenbox)

#Boost dependency
find_package(Boost REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})

#owwenbox dependency
link_directories(${owwenbox_BINARY_DIR})
include_directories(${owwenbox_INCLUDE_DIR})

#includes

#sources
set(ownullwenbox_SRCS
	NullWenbox.cpp
)

#owwenbox
add_library(ownullwenbox STATIC ${ownullwenbox_SRCS})

#owwenbox dependency
target_link_libraries(ownullwenbox owwenbox)
