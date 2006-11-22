# - OW_SET_LIBRARY_VERSION(build_version api_version)
# Sets the library version (build + api)
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_SET_LIBRARY_VERSION buildVersion apiVersion)

	ow_check_project()

	set(${PROJECT_NAME}_BUILD_VERSION
		${buildVersion}
	)

	set(${PROJECT_NAME}_API_VERSION
		${apiVersion}
	)

endmacro (OW_SET_LIBRARY_VERSION)
