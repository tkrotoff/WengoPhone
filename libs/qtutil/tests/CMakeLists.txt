# LinkLabel
ow_create_executable(testlinklabel)

ow_use_public_libraries(
	qt4
	owqtutil
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	testlinklabel.h
)

ow_add_sources(
	testlinklabel.cpp
)

ow_create_binary(MACOSX_BUNDLE EXCLUDE_FROM_ALL)

# ScrollFrame
ow_create_executable(testscrollframe)

ow_use_public_libraries(
	qt4
	owqtutil
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	testscrollframe.h
)

ow_add_sources(
	testscrollframe.cpp
)

ow_create_binary(MACOSX_BUNDLE EXCLUDE_FROM_ALL)

# ImageSelector
ow_create_executable(testimageselector)

ow_use_public_libraries(
	qt4
	owqtutil
)

ow_add_sources(
	testimageselector.cpp
)

ow_create_binary(MACOSX_BUNDLE EXCLUDE_FROM_ALL)

# DesktopService
ow_create_executable(desktopservice)

ow_use_public_libraries(
	qt4
	owqtutil
)

ow_add_sources(
	desktopservice.cpp
)

ow_create_binary(EXCLUDE_FROM_ALL)

