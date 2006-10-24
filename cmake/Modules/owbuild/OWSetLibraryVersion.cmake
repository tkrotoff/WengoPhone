# - OW_SET_LIBRARY_VERSION(build_version api_version)
# Sets the library version
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_SET_LIBRARY_VERSION _build_version _api_version)

	ow_check_project()

	set_target_properties(${PROJECT_NAME}
		PROPERTIES
			VERSION _build_version
			SOVERSION _api_version
	)

endmacro (OW_SET_LIBRARY_VERSION)
