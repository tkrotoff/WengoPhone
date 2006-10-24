# - OW_ADD_PRIVATE_COMPILE_FLAGS(flag1 ... flagN)
# Adds private compilation flags to the current project
#
# Copyright (C) 2006, Oswald Buddenhagen, <ossi@kde.org>
# Copyright (C) 2006, Andreas Schneider, <mail@cynapses.org>
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_ADD_PRIVATE_COMPILE_FLAGS)

	ow_check_project()

	get_target_property(${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS ${PROJECT_NAME} COMPILE_FLAGS)

	if (${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS STREQUAL "NOTFOUND")
		set(${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS
			${ARGN}
		)
	else (${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS STREQUAL "NOTFOUND")
		set(${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS
			${${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS}
			${ARGN}
		)
	endif (${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS STREQUAL "NOTFOUND")

	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS)

endmacro (OW_ADD_PRIVATE_COMPILE_FLAGS)
