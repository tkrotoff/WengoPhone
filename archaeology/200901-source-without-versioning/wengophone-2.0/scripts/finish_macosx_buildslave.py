#!/usr/bin/env python
#
# Sometimes buildbot on Mac OS X terminates the build process
# with a 'Device Not Configured' error coming from 'hdiutil'.
# This script finalize the build process by calling 'hdiutil'
# and sending the built application on the Wengo ftp server.
#
# Must be executed in the root directory
#
# @author Philippe Bernery
#

import os, sys

available_archs = [
	"ppc",
	"x86",
]

def usage():
	return "usage: \n" \
		+ sys.argv[0] + " [ppc|x86]"

if len(sys.argv) < 2:
	print usage()
	sys.exit(1)

arch = sys.argv[1]
if arch not in available_archs:
	print usage()
	sys.exit(1)

if os.path.exists("WengoPhone.dmg"):
	print "Removing old dmg... ",
	os.remove("WengoPhone.dmg")
	print "done"

hdiutilCommand = "hdiutil create -fs HFS+" \
	" -volname WengoPhone" \
	" -srcfolder release-symbols/wengophone/src/presentation/qt/WengoPhone.app" \
	" WengoPhone.dmg"

print "Executing hdiutil... ",
file = os.popen(hdiutilCommand, "r")
if file.close() == None:
	print "done"
	uploadCommand = "python scripts/upload_nightly.py" \
		" installer NG MacOSX" \
		" WengoPhone.dmg" \
		" WengoPhone-revrevnumber-" + arch + ".dmg"
	print "Executing upload_nightly.py... "
	file = os.popen(uploadCommand, "r")
	if file.close() == None:
		print "done"
	else:
		print "error!"
		sys.exit(1)
else:
	print "error!"
	sys.exit(1)

print "Finished successfully"
sys.exit(0)

