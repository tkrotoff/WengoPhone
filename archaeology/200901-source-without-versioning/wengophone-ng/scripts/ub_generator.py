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
# This script generates an Universal Binary version of a package.
# It currently works only with CoIpManage packages.
# It is done to work with OpenWengo buildbots.
# It reads a configuration file, given in first parameter of the script.
# This configuration must be like this:
#
# [general]
# projtype: <coipsdk|wengophone>
# ftpserver:
# ftplogin:
# ftppassword:
# ppcfile:
# x86file:
# ubpath:
# workdir:
# projfile:
import archiver, binary_utilities, ConfigParser, ftp_utilities, os, shutil, sys

USAGE = """
usage: ub_generator.py <config.ini>
"""
GENERAL_SECTION = "general"
PROJTYPE = "projtype"
COIP_PROJECT = "coipsdk"
WENGOPHONE_PROJECT = "wengophone"
FTPSERVER = "ftpserver"
FTPLOGIN = "ftplogin"
FTPPASSWORD = "ftppassword"
REVNUMBER = "revnumber"
PPCFILE = "ppcfile"
X86FILE = "x86file"
UBPATH = "ubpath"
WORKDIR = "workdir"
PROJFILE = "projfile"

def lipo(ppcOutDir, x86Dir):
	"""
	This methods will scan for all binary files in ppcDir and look for
	its x86 equivalent in x86Dir, then pass these libraries through lipo
	to generate a UB version.
	Generated file overwrite PPC ones.
	"""
	for root, dirs, files in os.walk(ppcOutDir):
		for f in files:
			relRoot = root[len(ppcOutDir) + 1:]
			ppcFile = os.path.join(ppcOutDir, relRoot, f)
			x86File = os.path.join(x86Dir, relRoot, f)

			if binary_utilities.is_binary_file(ppcFile):
				print "** Lipoing", ppcFile
				if not os.path.exists(x86File):
					print "!! file does not exist:", x86File
				elif binary_utilities.is_universal_binary_file(x86File):
					shutil.copy2(x86File, ppcFile)
				elif binary_utilities.is_universal_binary_file(ppcFile):
					# Already UB, nothing to do
					continue
				else:
					os.system("lipo -create %(ppcFile)s %(x86File)s -o %(ppcFile)s" \
						% { "ppcFile" : ppcFile, "x86File" : x86File });

def generate_ub_for_coipsdk(conf):
	"""
	This method generates an universal binary for CoIpManager SDK.
	Information about where to look for file and where to put result
	files are given in the Dictionnary conf.
	"""
	## Creates working directory
	print "** creating work dir..."
	ppcPath = os.path.join(conf[WORKDIR], "ppc")
	ppcFile = os.path.join(ppcPath, os.path.basename(conf[PPCFILE]))
	if not os.path.exists(ppcPath):
		os.makedirs(ppcPath)
	x86Path = os.path.join(conf[WORKDIR], "x86")
	x86File = os.path.join(x86Path, os.path.basename(conf[X86FILE]))
	if not os.path.exists(x86Path):
		os.makedirs(x86Path)
	####

	## Download files
	print "** downloading files..."
	ftp_utilities.download_file(conf[FTPSERVER], conf[FTPLOGIN],
		conf[FTPPASSWORD], conf[PPCFILE], ppcPath)
	ftp_utilities.download_file(conf[FTPSERVER], conf[FTPLOGIN],
		conf[FTPPASSWORD], conf[X86FILE], x86Path)
	####

	## Unarchive files
	print "** unpacking files..."
	ppcUnpackPath = os.path.join(ppcPath, "archive")
	archiver.unarchive_coipsdk(ppcFile, ppcUnpackPath)

	x86UnpackPath = os.path.join(x86Path, "archive")
	archiver.unarchive_coipsdk(x86File, x86UnpackPath)
	####

	## Generate UB
	print "** scanning file for lipo..."
	lipo(os.path.join(ppcUnpackPath, "CoIpManager.framework"),
		os.path.join(x86UnpackPath, "CoIpManager.framework"))
	####

	## Install UB SDK
	print "** removing old SDK..."
	if os.path.exists("/Library/Frameworks/CoIpManager.framework"):
		os.system("sudo rm -Rf /Library/Frameworks/CoIpManager.framework")

	print "** Copying generated framework in /Library/Frameworks..."
	os.system("sudo cp -RP %(srcDir)s %(dstDir)s" \
		% { "srcDir" : os.path.join(ppcUnpackPath, "CoIpManager.framework"), "dstDir" : "/Library/Frameworks/" })
	####

	## Archive the installed SDK
	print "** archiving framework..."
	ubPackage = os.path.join(conf[WORKDIR], "CoIpManager-" +  conf[REVNUMBER] + ".pkg.tar.bz2")
	archiver.archive_coipsdk(os.path.join(ppcUnpackPath, "CoIpManager-0.2.pkg"), conf[PROJFILE], ubPackage)
	####

	## Upload to FTP server
	print "** uploading UB package..."
	ftp_utilities.upload_file(conf[FTPSERVER], conf[FTPLOGIN],
		conf[FTPPASSWORD], conf[UBPATH], ubPackage)
	####

def generate_ub_for_wengophone(conf):
	"""
	This method generates an universal binary for WengoPhone.
	Information about where to look for file and where to put result
	files are given in the Dictionnary conf.
	"""
	## Creates working directory
	print "** creating work dir..."
	ppcPath = os.path.join(conf[WORKDIR], "ppc")
	ppcFile = os.path.join(ppcPath, os.path.basename(conf[PPCFILE]))
	if not os.path.exists(ppcPath):
		os.makedirs(ppcPath)
	x86Path = os.path.join(conf[WORKDIR], "x86")
	x86File = os.path.join(x86Path, os.path.basename(conf[X86FILE]))
	if not os.path.exists(x86Path):
		os.makedirs(x86Path)
	####

	## Download files
	print "** downloading files..."
	ftp_utilities.download_file(conf[FTPSERVER], conf[FTPLOGIN],
		conf[FTPPASSWORD], conf[PPCFILE], ppcPath)
	ftp_utilities.download_file(conf[FTPSERVER], conf[FTPLOGIN],
		conf[FTPPASSWORD], conf[X86FILE], x86Path)
	####

	## Unarchive files
	print "** unpacking files..."
	ppcUnpackPath = os.path.join(ppcPath, "archive")
	archiver.unarchive_wengophone(ppcFile, ppcUnpackPath)

	x86UnpackPath = os.path.join(x86Path, "archive")
	archiver.unarchive_wengophone(x86File, x86UnpackPath)
	####

	## Generate UB
	print "** scanning file for lipo..."
	lipo(os.path.join(ppcUnpackPath, "WengoPhone.app"),
		os.path.join(x86UnpackPath, "WengoPhone.app"))
	####

	## Archive the updated WengoPhone
	print "** archiving UB WengoPhone..."
	ubPackage = os.path.join(conf[WORKDIR], "WengoPhone-" +  conf[REVNUMBER] + ".dmg")
	archiver.archive_wengophone(os.path.join(ppcUnpackPath, "WengoPhone.app"), ubPackage)
	####

	## Upload to FTP server
	print "** uploading UB package..."
	ftp_utilities.upload_file(conf[FTPSERVER], conf[FTPLOGIN],
		conf[FTPPASSWORD], conf[UBPATH], ubPackage)
	####

if __name__ == "__main__":
	if len(sys.argv) < 2:
		print USAGE
		sys.exit(1)

	config = ConfigParser.SafeConfigParser()
	config.read([sys.argv[1]])

	if config.has_option(GENERAL_SECTION, PROJTYPE):
		myDict = dict(config.items(GENERAL_SECTION))
		if myDict[PROJTYPE] == COIP_PROJECT:
			generate_ub_for_coipsdk(myDict)
		elif myDict[PROJTYPE] == WENGOPHONE_PROJECT:
			generate_ub_for_wengophone(myDict)
		else:
			print "!! unknown project type specified. Check .ini file syntax."
			sys.exit(1)
	else:
		print "!! no project type specified. Check .ini file syntax."
		sys.exit(1)
