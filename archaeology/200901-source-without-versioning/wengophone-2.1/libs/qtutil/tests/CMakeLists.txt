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

# testwidgetratioenforcer
ow_create_executable(testwidgetratioenforcer)

ow_use_public_libraries(
	qt4
	owqtutil
)

ow_add_sources(
	testwidgetratioenforcer.cpp
)

ow_create_binary(MACOSX_BUNDLE EXCLUDE_FROM_ALL)

# testqobjectthreadsafe
ow_create_executable(testqobjectthreadsafe)

ow_use_public_libraries(
	qt4
	owqtutil
	owutil
)

ow_add_sources(
	testqobjectthreadsafe.cpp
)

ow_create_binary(MACOSX_BUNDLE EXCLUDE_FROM_ALL)

# testexpandbutton
ow_create_executable(testexpandbutton)

ow_use_public_libraries(
	qt4
	owqtutil
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	testexpandbutton.h
)

ow_add_sources(
	testexpandbutton.cpp
)

ow_create_binary(MACOSX_BUNDLE EXCLUDE_FROM_ALL)

# testflashwindow
ow_create_executable(testflashwindow)

ow_use_public_libraries(
	qt4
	owqtutil
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	testflashwindow.h
)

ow_add_sources(
	testflashwindow.cpp
)

ow_create_binary(MACOSX_BUNDLE EXCLUDE_FROM_ALL)