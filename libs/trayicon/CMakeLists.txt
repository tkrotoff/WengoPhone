project(owtrayicon)

#Qt4 dependency
find_package(Qt4 REQUIRED)
include_directories(
	${QT_INCLUDE_DIR}
)

#includes
include_directories(${owtrayicon_SOURCE_DIR})

#sources
set(owtrayicon_SRCS
	trayicon.cpp
)

if (WIN32)
	set(owtrayicon_SRCS ${owtrayicon_SRCS}
		trayicon_win.cpp
	)
endif (WIN32)

if (UNIX)
	if (APPLE)
		set(owtrayicon_SRCS ${owtrayicon_SRCS}
			trayicon_mac.cpp
		)
	else (APPLE)
		set(owtrayicon_SRCS ${owtrayicon_SRCS}
			trayicon_x11.cpp
		)
	endif (APPLE)
endif (UNIX)

qt4_automoc(${owtrayicon_SRCS})

#owtrayicon
add_library(owtrayicon STATIC ${owtrayicon_SRCS})
