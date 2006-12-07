# Global public variables
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_DIR)
set(BUILD_DIR
	${CMAKE_BINARY_DIR}/${BUILD_DIR}
	CACHE PATH "Build directory, depends on build type" FORCE
)

set(LOCALE_INSTALL_DIR
	"${BUILD_DIR}/lang"
	CACHE PATH "Location for translations (*.qm files)" FORCE
)
file(MAKE_DIRECTORY ${LOCALE_INSTALL_DIR})

# Gets svn revision
set(SVN_REVISION "0")
ow_get_subversion_revision(SVN_REVISION)
# Bugfix with svn revision number that can integrate a : and
# this does not work under Windows for the installer name, replace it by -
string(REPLACE ":" "-" SVN_REVISION ${SVN_REVISION})
