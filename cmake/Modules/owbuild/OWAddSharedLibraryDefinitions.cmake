# - OW_ADD_SHARED_LIBRARY_DEFINITIONS()
# Internal function, adds shared library definitions declspec(dllimport) and declspec(dllexport)
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_ADD_SHARED_LIBRARY_DEFINITIONS)

	string(TOUPPER ${PROJECT_NAME} tmp)

	ow_add_public_definitions(
		-D${tmp}_DLL
	)

	ow_add_private_definitions(
		-D${tmp}_DLL
		-DBUILD_${tmp}_DLL
	)

endmacro (OW_ADD_SHARED_LIBRARY_DEFINITIONS)
