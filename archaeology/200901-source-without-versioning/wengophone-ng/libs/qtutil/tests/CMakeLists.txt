# LinkLabel
ow_create_executable(testlinklabel)

ow_use_public_libraries(
	owqtutil
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	testlinklabel.h
)

ow_add_sources(
	testlinklabel.cpp
)

ow_create_project_binary(MACOSX_BUNDLE)


# ScrollFrame
ow_create_executable(testscrollframe)

ow_use_public_libraries(
	owqtutil
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	testscrollframe.h
)

ow_add_sources(
	testscrollframe.cpp
)

ow_create_project_binary(MACOSX_BUNDLE)


# ImageSelector
ow_create_executable(testimageselector)

ow_use_public_libraries(
	owqtutil
)

ow_add_sources(
	testimageselector.cpp
)

ow_create_project_binary(MACOSX_BUNDLE)


# DesktopService
ow_create_executable(desktopservice)

ow_use_public_libraries(
	owqtutil
)

ow_add_sources(
	desktopservice.cpp
)

ow_create_project_binary(MACOSX_BUNDLE)


# testwidgetratioenforcer
ow_create_executable(testwidgetratioenforcer)

ow_use_public_libraries(
	owqtutil
)

ow_add_sources(
	testwidgetratioenforcer.cpp
)

ow_create_project_binary(MACOSX_BUNDLE)


# testexpandbutton
ow_create_executable(testexpandbutton)

ow_use_public_libraries(
	owqtutil
)

ow_add_sources(
	testexpandbutton.cpp
)

ow_create_project_binary(MACOSX_BUNDLE)
