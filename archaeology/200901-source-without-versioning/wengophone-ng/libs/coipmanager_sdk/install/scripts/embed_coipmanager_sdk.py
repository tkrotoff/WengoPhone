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
# This script copies CoIpManager into an application bundle and change
# install name of the application binary.
#

import binary_utilities, os, shutil, sys

USAGE='''
usage: embed_coipmanager_sdk.py <application.app> <path_to_sdk>
e.g.: embed_coipmanager_sdk.py WengoPhone.app /Library/Frameworks
'''

def embedCoIpManagerSDK(appPath, sdkPath):
	'''
	Embed CoIpManager SDK in application bundle.
	1 - copy the SDK in appPath/Contents/Frameworks
	2 - change install name of appPath/Contents/MacOS/app
	3 - change install name of appPath/Contents/Frameworks/CoIpManager.framework/Libraries/*
	'''
	print "Copying SDK..."
	pathToCopiedSDK = os.path.join(appPath, "Contents", "Frameworks")
	pathWithSDK = os.path.join(pathToCopiedSDK, os.path.basename(sdkPath))
	shutil.rmtree(pathWithSDK, True)
	try:
		os.makedirs(pathToCopiedSDK)
	except Exception, e:
		pass
	shutil.copytree(sdkPath, pathWithSDK, True)

	applicationName = os.path.basename(appPath).split('.')[0]
	applicationPath = os.path.join(appPath, "Contents", "MacOS", applicationName)
	print "Changing install name on", applicationName, "..."
	link_to_coipmanager_sdk.linkToCoIpManagerSDK(applicationPath,
		"@executable_path/../Frameworks/CoIpManager.framework/")

	print "Changing install name on SDK", pathToCopiedSDK, "..."
	change_install_name.changeInstallNameInDir(pathToCopiedSDK,
		"@executable_path/../Frameworks/CoIpManager.framework/Versions/Current/Libraries/")

if __name__ == "__main__":
	if len(sys.argv) < 3:
		print USAGE
		sys.exit(1)

	embedCoIpManagerSDK(sys.argv[1], sys.argv[2])
