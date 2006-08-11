project(owbrowser)

#Qt4 dependency
find_package(Qt4 REQUIRED)
include_directories(
	${QT_INCLUDE_DIR}
)

#owqtutil dependency
include_directories(${owqtutil_INCLUDE_DIR})

#includes
include_directories(${owbrowser_SOURCE_DIR})

#sources
set(owbrowser_SRCS
	QtBrowser.cpp
)

qt4_automoc(${owbrowser_SRCS})

#owidle
add_library(owbrowser STATIC ${owbrowser_SRCS})

#owqtutil dependency
target_link_libraries(owbrowser owqtutil)
