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
# @author Ovidiu Ciule
# @author Philippe Bernery

from PyCoIpManager import *

from PyQt4 import QtCore

from util.SafeConnect import *

from ConfigParser import SafeConfigParser
import os, platform, sys, logging

log = logging.getLogger("PyWengoPhone.Config")

GENERAL_SECTION = "general"
LASTUSEDUSERPROFILE_OPTION = "lastuseduserprofile"
FILETRANSFER_SECTION = "filetransfer"
FILETRANSFERDOWNLOADFOLDER_OPTION = "filetransferdownloadfolder"
FILETRANSFERLASTUPLOADEDFILEFOLDER_OPTION = "filetransferlastuploadedfilefolder"

class Config:
    __configFilename = "PyWengoPhone.conf"

    __currentInstance = None
    def getInstance():
        if Config.__currentInstance == None:
            Config.__currentInstance = Config()
        return Config.__currentInstance
    getInstance = staticmethod(getInstance)

    __slots__ = [
        "lastUsedUserProfile",
        "resourcesFolder",
        "fileTransferDownloadFolder",
        "fileTransferLastUploadedFileFolder",
        "configurationFolder",
        "configurationFilename",
        "companyWebSiteUrl"
    ]

    def __init__(self):
        if Config.__currentInstance != None:
            # Should not get here, constructor should not be called directly
            raise RuntimeError, "Constructor has been called twice. Probably \
                it has been called directly.\n This is a Singleton. Please \
                use getInstance()"
            return

        self.__setDefaultValues()
        self.load()

    def __getConfigurationFolder(self):
        """
        Gets the configuration directory path.
        It is usually:
        On Windows: C:\\Documents and Settings\\username\\Application Data\\PyWengoPhone\\
        On Mac OS X: $HOME/Library/Application Support/PyWengoPhone/
        On Linux: $HOME/.pywengophone
        """
        dirName = ".pywengophone" # For Linux
        if (platform.system() == "Darwin") or (platform.system() == "Windows"):
            dirName = "PyWengoPhone"
        return os.path.join(Path.getConfigurationDirPath(), dirName + str(os.path.sep))

    def __getConfigurationFilename(self):
        return os.path.join(self.configurationFolder, self.__configFilename)

    def __setDefaultValues(self):
        """
        Sets default values on config. members.
        """
        self.companyWebSiteUrl = ""
        self.lastUsedUserProfile = ""
        self.__setResourcesFolder()
        self.fileTransferDownloadFolder = ""
        self.fileTransferLastUploadedFileFolder = ""

    configurationFolder = property(__getConfigurationFolder)
    configurationFilename = property(__getConfigurationFilename)

    def load(self):
        """
        Load last config. file.
        """
        try:
            config = SafeConfigParser()
            config.read([self.configurationFilename])
            if config.has_section(GENERAL_SECTION):
                if config.has_option(GENERAL_SECTION, LASTUSEDUSERPROFILE_OPTION):
                    self.lastUsedUserProfile = config.get(GENERAL_SECTION, LASTUSEDUSERPROFILE_OPTION)
            if config.has_section(FILETRANSFER_SECTION):
                if config.has_option(FILETRANSFER_SECTION, FILETRANSFERDOWNLOADFOLDER_OPTION):
                    self.fileTransferDownloadFolder = config.get(FILETRANSFER_SECTION, FILETRANSFERDOWNLOADFOLDER_OPTION)
                if config.has_option(FILETRANSFER_SECTION, FILETRANSFERLASTUPLOADEDFILEFOLDER_OPTION):
                    self.fileTransferLastUploadedFileFolder = config.get(FILETRANSFER_SECTION, FILETRANSFERLASTUPLOADEDFILEFOLDER_OPTION)
        except IOError:
            pass

    def save(self):
        """
        Saves current config. file.
        """
        if not os.path.exists(self.configurationFolder):
            os.mkdir(self.configurationFolder)

        config = SafeConfigParser()
        config.add_section(GENERAL_SECTION)
        config.set(GENERAL_SECTION, LASTUSEDUSERPROFILE_OPTION, self.lastUsedUserProfile)
        config.add_section(FILETRANSFER_SECTION)
        config.set(FILETRANSFER_SECTION, FILETRANSFERDOWNLOADFOLDER_OPTION, self.fileTransferDownloadFolder)
        config.set(FILETRANSFER_SECTION, FILETRANSFERLASTUPLOADEDFILEFOLDER_OPTION, self.fileTransferLastUploadedFileFolder)
        config.write(open(self.configurationFilename, "w"))

    def __setResourcesFolder(self):
        """
        Retrieves the Resources directory path.
        It is usually:
        On Windows: C:\\Program Files\\WengoPhone\\
        On Mac OS X: $APPLICATIONS/WengoPhone.app/Contents/Resources
        On Linux: $PREFIX/share/wengophone; don't ask what that prefix is
        This resources path can also be set on PyWengoPhone command line with
        the --resources= option.
        """
        if platform.system() == "Windows":
            log.debug("__setResourcesFolder: platform = Windows")
            self.resourcesFolder = Path.getApplicationDirPath()
        elif platform.system() == "Darwin":
            log.debug("__setResourcesFolder: platform = Darwin")
            self.resourcesFolder = Path.getApplicationResourcesDirPath()
        elif platform.system() == "Linux":
            log.debug("__setResourcesFolder: platform = Linux")
            self.resourcesFolder = "/usr/local/share/wengophone"
        else:
            log.debug("getResourcesDirPath: platform detection failed; \
                propably this platform is not supported; platform.system()=" + platform.system)

        # Check if dir exist
        # If not, resourcesFolder is set to sys.path[0]
        if not os.path.exists(self.resourcesFolder):
            log.debug("__setResourcesFolder: path \"" + self.resourcesFolder + "\" does not exist, using sys.path[0]")
            self.resourcesFolder = sys.path[0]
        log.debug("__setResourcesFolder: path=\"" + self.resourcesFolder + "\"")
