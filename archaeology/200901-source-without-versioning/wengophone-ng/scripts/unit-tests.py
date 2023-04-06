#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# WengoPhone, a voice over Internet phone
# Copyright (C) 2004-2007  Wengo
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# @author Philippe Bernery <philippe.bernery@wengo.com>
#
# This script executes all unit-tests builded by the target 'unit-tests'
# Exit with status 0 if successfull, 1 otherwise
#
import os, sys

USAGE="""
usage: unit-tests.py <path_to_unit_tests>
"""

def get_test_list():
	"""
	TODO: get test list from the unit-tests CMakeLists.txt
	"""
	return [
		"coipmanager-unit-tests",
		"coipmanager_base-unit-tests",
		"owsettings-unit-tests",
		"owutil-unit-tests",
		"webcam-unit-tests"
	]

def unit_tests(path):
	"""
	executes all unit-tests produced by the OWBuild target 'unit-tests'
	@return True if succesful, False otherwise
	"""
	testPrograms = get_test_list()
	for test in testPrograms:
		command = os.path.join(path, test)
		print "Executing", command
		if os.system(command) != 0:
			return False
	return True

if __name__ == "__main__":
	if len(sys.argv) < 2:
		print USAGE
		sys.exit(1)

	if unit_tests(sys.argv[1]):
		print "Unit tests successful!"
		sys.exit(0)
	else:
		print "Error :("
		sys.exit(1)
