# - ow_hdiutil(volname src dst)
# Mac OS X only.
#  calls "hdiutil create -fs HFS+ -volname ${volname} -srcfolder ${src} ${dst}"
#
# hdiutil creates a .dmg file.
#
# Copyright (C) 2006-2007  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


if (APPLE)
	macro (ow_hdiutil volname src dst)

		exec_program("hdiutil"
			ARGS
				"create -fs HFS+ -volname ${volname} -srcfolder ${src} ${dst}"
		)

	endmacro (ow_hdiutil)
endif (APPLE)
