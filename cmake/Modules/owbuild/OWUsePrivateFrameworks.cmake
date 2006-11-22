# - OW_USE_PRIVATE_FRAMEWORKS(framework1 ... frameworkN)
# Uses privately a framework inside the current project:
# imports properties from a framework to the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


if (APPLE)
	include (CMakeFindFrameworks)

	macro (OW_USE_PRIVATE_FRAMEWORKS)

		ow_check_project()

		foreach (fwk ${ARGN})
			cmake_find_frameworks(${fwk})

			set(first ${${fwk}_FRAMEWORKS})

			ow_add_private_link_flags(
				"-framework ${fwk}"
			)
		endforeach (fwk ${ARGN})

	endmacro (OW_USE_PRIVATE_FRAMEWORKS)
endif (APPLE)
