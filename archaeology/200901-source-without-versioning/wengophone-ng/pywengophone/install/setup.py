#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# WengoPhone, a voice over Internet phone
# Copyright (C) 2004-2007 Wengo
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
# Test program for PyCoIpManager
#
# @author Philippe Bernery

from distutils.core import setup
from distutils.extension import Extension
import os
import platform
import shutil
import sys

mainscript = "PyWengoPhone.py"

moduleList = [
    "MainWindow",
    "MainWindow_ui",
	"wengophone_rc",
]

packageList = [
    "account",
    "call",
    "chat",
    "config",
    "contact",
    "profile",
    "userprofile",
    "util",
]

rootPath = os.path.join(os.curdir, "..", "src")
if os.path.exists("dist"):
	shutil.rmtree("dist")
	os.makedirs("dist")

if platform.system() == "Darwin":
    # Generates the PyWengoPhone.app for Mac OS X

    # Embarquer CoIpManager.framework
    # Créer fichier Info.plist

    print "** create needed directories..."
    frameworksDir = "dist/PyWengoPhone.app/Contents/Frameworks"
    macosDir = "dist/PyWengoPhone.app/Contents/MacOS"
    resourcesDir = "dist/PyWengoPhone.app/Contents/Resources"
    dirs = [
        frameworksDir,
        macosDir,
        resourcesDir,
    ]
    for d in dirs:
        if not os.path.exists(d):
            os.makedirs(d)

    print "** copy python scripts..."
    for root, dirs, files in os.walk(rootPath):
		for f in files:
			myFile = os.path.join(root, f)
			if myFile.endswith(".py"):
			    relFile = myFile[len(rootPath) + 1:]
			    contentDir = os.path.join(resourcesDir, os.path.dirname(relFile))
			    if not os.path.exists(contentDir):
			        os.makedirs(contentDir)
			    shutil.copy(myFile, os.path.join(resourcesDir, relFile))

    print "** create launcher script..."
    scriptCode = "#!/bin/sh\n\
CURDIR=$(dirname $0)\n\
echo $CURDIR\n\
export PYTHONPATH=$CURDIR/../Frameworks/CoIpManager.framework/Libraries\n\
$CURDIR/python $CURDIR/../Resources/PyWengoPhone.py\n\
    "
    scriptFileName = os.path.join(macosDir, "PyWengoPhone")
    scriptFile = open(scriptFileName, "w")
    scriptFile.write(scriptCode)
    scriptFile.close()
    os.system("chmod ug+x " + scriptFileName)

    print "** create link to python..."
    os.system("ln -s /System/Library/Frameworks/Python.framework/Versions/Current/bin/python " + macosDir)

    print "** embed CoIpManager..."
    # Copy CoIpManager from /Library/Frameworks
    os.system("cp -RP /Library/Frameworks/CoIpManager.framework " + frameworksDir)
	#shutil.copytree("/Library/Frameworks/CoIpManager.framework", frameworksDir, True)
    # Change its install names
    os.system("python ../../scripts/update_links.py " + frameworksDir + " " + scriptFileName)

    ## Create Info.plist
    ####
elif platform.system() == "Windows" or platform.system() == "Linux":
	if platform.system() == "Windows":
		import py2exe
		extra_options = dict(
			zipfile=None,
			windows=[os.path.join(rootPath, mainscript)],
			options={"py2exe":{"includes":["sip"]}}
		)
		#print "** copy of CoIpManager plugins and dependencies..."
		#shutil.copytree("C:/CoIpManager-0.2/lib/", "dist/")

	setup(name="PyWengoPhone",
		version="0.2",
		description="A Voice Over IP phone",
		author="Philippe Bernery",
		author_email="philippe.bernery@wengo.com",
		url="http://dev.openwengo.org",
		py_modules=moduleList,
		packages=packageList,
		package_dir={"" : rootPath},
		**extra_options
		)
