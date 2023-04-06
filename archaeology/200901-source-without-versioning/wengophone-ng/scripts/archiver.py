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
# This script gathers utiliy functions to unarchive OpenWengo files
# (CoIpManager SDK, WengoPhone, ...)
# Made for Mac OS X
import os, tarfile

def unarchive_coipsdk(myFile, outdir):
	"""
	Unpacks myFile in outdir.
	"""
	print "** unarchiving %(myFile)s in %(outdir)s..."  % {"myFile": myFile, "outdir": outdir}
	taro = tarfile.open(myFile, "r:bz2")
	for member in taro.getmembers():
		print "** extracting", member.name
		taro.extract(member, outdir)

	print "** unpaxing files..."
	workdir = os.path.join(outdir, "CoIpManager.framework")
	if not os.path.exists(workdir):
		os.makedirs(workdir)
	pathToArchive = os.path.join(outdir, "minsizerel", "CoIpManager-0.2.pkg", "Contents", "Archive.pax.gz")
	os.system("cd %(workdir)s && pax -rzf %(archive)s" \
		% { "archive": pathToArchive, "workdir": workdir })

def archive_coipsdk(pkgDir, projFile, outBzipFile):
	"""
	Create a .pkg of a CoIpManager.framework directory then
	"tar bzip" it.
	"""
	print "** calling package maker..."
	os.system("sudo /Developer/Tools/packagemaker -build -v -p %(pkgDir)s -proj %(projFile)s"
		% { "pkgDir" : pkgDir, "projFile" : projFile })

	print "** tar-bzipping .pkg..."
	os.system("tar -cjv -C %(workdir)s -f %(out)s %(dir)s" \
		% { "workdir" : os.path.dirname(pkgDir),
			"out" : outBzipFile,
			"dir" : os.path.basename(pkgDir) })

def unarchive_wengophone(myFile, outdir):
	"""
	Unarchives a .dmg file.
	"""
	os.system("hdiutil mount %s" % myFile)
	os.system("cp -RP /Volumes/WengoPhone %s" % outdir)
	os.system("hdiutil eject /Volumes/WengoPhone")

def archive_wengophone(myDir, outfile):
	"""
	Archives a directory in a .dmg file.
	"""
	os.system("hdiutil create -fs HFS+ -volname WengoPhone -srcfolder %(srcfolder)s %(outfile)s" \
		% { "srcfolder" : myDir, "outfile" : outfile })
