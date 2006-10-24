# - OW_CREATE_EXECUTABLE()
# Creates an executable (.exe) using the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_CREATE_EXECUTABLE)

	macro_prepare_binary()

	add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})

	target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_PRIVATE_LIBRARIES})

	install(TARGETS ${PROJECT_NAME} DESTINATION bin/)

endmacro (OW_CREATE_EXECUTABLE)
