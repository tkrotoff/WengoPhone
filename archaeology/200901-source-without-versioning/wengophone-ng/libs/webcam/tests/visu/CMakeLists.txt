ow_create_executable(webcam-test-visu)

ow_use_private_libraries(
	webcam
	qtgui
)

ow_add_sources(
	webcam_test.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	webcam_test.h
)

ow_create_project_binary(MACOSX_BUNDLE)
