#!/usr/bin/env python
# vim: noexpandtab
import commands
import re
import sys

from path import path

USAGE="""
Usage: copy_gnulinux_libs.py /path/to/some/binary

Copies all the necessary .so files for 'binary' to '/path/to/some'.
This is useful to gather all files necessary for a nightly tarball.
"""


# This is a list of libs we consider to be already installed on all machines,
# and thus which should not be included in the nightly tarball.
IGNORED_LIB_LIST = [
	"ICE",
	"SM",
	"X11",
	"Xau",
	"Xdmcp",
	"Xext",
	"Xfixes",
	"Xi",
	"Xinerama",
	"Xrandr",
	"Xrender",
	"c",
	"dl",
	"gcc_s",
	"m",
	"nsl",
	"pthread",
	"resolv",
	"rt",
	"stdc++",
]


def parseLdd(binary):
	"""
	Run ldd on a binary and returns a dict of libraries. Dictionary looks like
	this:
	abc: /lib/libabc.so.12
	xyz: /usr/lib/libxyz.so.34
	"""
	result = {}

	status, output = commands.getstatusoutput("ldd " + binary)

	if status != 0:
		raise Exception("No such binary: " + binary)

	rx = re.compile("lib([-+_a-zA-Z0-9.]+)\.so.* => ([^ ]+)")

	for line in output.split("\n"):
		match = rx.search(line)
		if not match:
			continue
		name = match.group(1)
		libPath = match.group(2)

		lib = match.group(1)
		result[lib] = path(libPath)

	return result


def main(args):
	if len(args) != 2:
		print USAGE
		return 1

	binary = path(args[1])
	assert binary.exists()
	libPathDict = parseLdd(binary)

	binaryDir = binary.dirname()
	for libName, libPath in libPathDict.items():
		if libName in IGNORED_LIB_LIST:
			continue

		print "Copying %s" % libName 
		libPath.copy(binaryDir)

	return 0

if __name__ == "__main__":
	sys.exit(main(sys.argv))
