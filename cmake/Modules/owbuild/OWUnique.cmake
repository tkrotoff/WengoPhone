# - OW_UNIQUE(unique_list list)
# Makes the given list have only one instance of each unique element
# See http://voxel.jouy.inra.fr/darcs/contrib-itk/WrapITK-unstable/CMakeUtilityFunctions.cmake
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_UNIQUE unique_list)
	set(unique_tmp
		""
	)

	foreach (loop ${ARGN})
		if (NOT "${unique_tmp}" MATCHES "(^|;)${loop}(;|$)")
			set(unique_tmp
				${unique_tmp}
				${loop}
			)
		endif (NOT "${unique_tmp}" MATCHES "(^|;)${loop}(;|$)")
	endforeach (loop)

	set(${unique_list}
		${unique_tmp}
	)
endmacro (OW_UNIQUE)
