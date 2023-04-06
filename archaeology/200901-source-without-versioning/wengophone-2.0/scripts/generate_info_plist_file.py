#!/usr/bin/env python
# -*- coding: utf-8 -*- 

# generate_info_plist_file.py
# OpenWengo
#
# Created by Philippe BERNERY on 15/09/06.
#
# Generates the Info.plist for WengoPhone
#
# Takes 2 arguments:
#	- folder where to put the Info.plist file
#	- version number of WengoPhone
#

import codecs
import os
import sys

##
# FUNCTION DEFINITION PART
##

##
# @return a string containing the usage message
def usage():
	return "usage: generate_info_plist_file.py "\
	"[Info.plist folder] [WengoPhone version number]"

##
# SCRIPT PART
##
if len(sys.argv) < 2:
    print usage()
    sys.exit(1)

infoPListFolder = sys.argv[1]
versionNumber = sys.argv[2]

fileContent = \
'<?xml version="1.0" encoding="UTF-8"?>\n' \
'<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">\n' \
'<plist version="1.0">\n' \
'<dict>\n' \
'	<key>CFBundleDevelopmentRegion</key>\n' \
'	<string>English</string>\n' \
'	<key>CFBundleExecutable</key>\n' \
'	<string>WengoPhone</string>\n' \
'	<key>CFBundleIconFile</key>\n' \
'	<string>wengophone.icns</string>\n' \
'	<key>CFBundleIdentifier</key>\n' \
'	<string>com.openwengo.WengoPhone</string>\n' \
'	<key>CFBundleInfoDictionaryVersion</key>\n' \
'	<string>6.0</string>\n' \
'	<key>CFBundlePackageType</key>\n' \
'	<string>APPL</string>\n' \
'	<key>CFBundleSignature</key>\n' \
'	<string>WNGP</string>\n' \
'	<key>CFBundleURLTypes</key>\n' \
'	<array>\n' \
'		<dict>\n' \
'			<key>CFBundleTypeRole</key>\n' \
'			<string>Viewer</string>\n' \
'			<key>CFBundleURLName</key>\n' \
'			<string>Wengo Me URL</string>\n' \
'			<key>CFBundleURLSchemes</key>\n' \
'			<array>\n' \
'				<string>wengo</string>\n' \
'			</array>\n' \
'		</dict>\n' \
'	</array>\n' \
'	<key>CFBundleVersion</key>\n' \
'	<string>' + versionNumber + '</string>\n' \
'</dict>\n' \
'</plist>\n'

unicodeFileContent = unicode(fileContent)
fileObj = codecs.open(os.path.join(infoPListFolder, "Info.plist"), "w", "utf-8")
fileObj.write(unicodeFileContent)

print "Info.plist write successful!"
