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
# FTP operations utilities.
import ftplib, os

def download_file(server, login, password, path, outfiledir):
	"""
	This method download a file from a FTP server.
	"""
	print "** downloading ftp://%(login)s@%(server)s%(path)s in %(outfiledir)s..." \
		% { "login": login, "server": server, "path": path, "outfiledir": outfiledir }

	outfile = open(os.path.join(outfiledir, os.path.basename(path)), "wb")
	ftp = ftplib.FTP(server)
	ftp.login(login, password)
	ftp.retrbinary("RETR " + path, outfile.write)
	ftp.quit()

def upload_file(server, login, password, serverpath, filetoupload):
	"""
	This method upload a file onto a FTP server.
	"""
	print "** uploading %(filetoupload)s in ftp://%(login)s@%(server)s%(serverpath)s..." \
		% { "filetoupload" : filetoupload, "login": login, "server": server, "serverpath": serverpath }

	infile = open(filetoupload, "rb")
	ftp = ftplib.FTP(server)
	ftp.login(login, password)
	ftp.storbinary("STOR " + os.path.join(serverpath, os.path.basename(filetoupload)), infile)
	ftp.quit()
