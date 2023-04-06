# -*- coding: utf-8 -*-
#
# WengoPhone, a voice over Internet phone
# Copyright (C) 2004-2007 Wengo
#
# This program is free software you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with self program if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
# @author Aurelien Gateau
# @author Ovidiu Ciule

from Emoticon import *
from util.SafeConnect import *
from config import Config
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtXml import *
import os, logging
log = logging.getLogger("PyWengoPhone.EmoticonsWidget")


class EmoticonsManager:
    """
    Emoticons Manager provides utility functions for emoticon manipulation
    Ported from WengoPhone 2.2
    """

    __currentInstance = None
    def getInstance():
        if EmoticonsManager.__currentInstance == None:
            EmoticonsManager.__currentInstance = EmoticonsManager()
        return EmoticonsManager.__currentInstance
    getInstance = staticmethod(getInstance)

    def __init__(self):
        if EmoticonsManager.__currentInstance!=None:
            # Should not get here, constructor should not be called directly
            raise RuntimeError, "Constructor has been called twice. Probably \
                it has been called directly.\n This is a Singleton. Please \
                use getInstance()"
            return
        self.__resDir = Config.getInstance().resourcesFolder
        self.__protocolList = {}
        self.loadFromFile(os.path.join(self.__resDir, "pics", "emoticons", "icondef.xml"))

    def getProtocolCount():
        return len(self.__protocolList)

    def getEmoticonCount(self, protocol):
        if not protocol in self.__protocolList:
            return 0
        return len(self.__protocolList[protocol])

    def getEmoticonList(self, protocol):
        if protocol in self.__protocolList:
            return self.__protocolList[protocol]
        return []

    def loadFromFile(self, filename):
        """
        Parses a .xml file and reads image files from disk and loads the emoticons in memory.
        """
        ErrorMsg = ""
        ErrorLine = 0
        ErrorCol = 0
        doc = QDomDocument("wengoIcons")
        try:
            file = open(filename, "r")
        except IOError:
            log.error("loadFromFile: Resource directory not found, skipping emoticons")
            return

        content = "".join(file.readlines())

        (done, ErrorMsg, ErrorLine, ErrorCol) = doc.setContent(content)
        file.close()
        if not done:
            return

        tmpElement = QDomElement()
        docElem = doc.documentElement()
        n = docElem.firstChild()
        tagName = ""

        # TODO This doesn't look very Pythonic; it should be rewritten
        while not n.isNull():
            tmpElement = n.toElement()
            if not tmpElement.isNull():
                tagName = tmpElement.tagName()
                if (tagName == "protocol"):
                    # A protocol tag was found; we'll read it using readProtocol()
                    self.readProtocol(n)
            n = n.nextSibling()

    def readProtocol(self, node):
        """ Parses a protocol node
            Identifies image nodes and reads them using readIcon
        """
        n = node.firstChild()
        element = QDomElement()
        attributeName = ""
        while not n.isNull():
            if not n.toElement().isNull():
                if n.toElement().tagName() == "icon":
                    # This is an icon node
                    element = node.toElement()
                    if not element.isNull():
                        attributeName = element.attribute("name")
                        self.readIcon(n, attributeName)
            n = n.nextSibling()

    def readIcon(self, node, protocol):
        """ Parses icon nodes
            Reads icon file paths and emoticon text represantations from the node
            Then reads the icon file, loads it in memory as an QPixmap
            Creates and sets up an emoticon object
        """
        protocol = str(protocol)
        n1 = node.firstChild()
        textList = []
        emoticon = Emoticon()
        while not n1.isNull():
            e1 = n1.toElement()
            if not e1.isNull():
                if e1.tagName() == "text":
                    textList.append(str(e1.text()))
                if (e1.tagName() == "object"):
                    emoticonPath = os.path.join(self.__resDir, "pics", "emoticons", str(e1.text()))
                    emoticon.setPath(emoticonPath)
                    emoticonPix = QPixmap(emoticonPath)
                    emoticon.setPixmap(emoticonPix)
                    emoticon.setButtonPixmap(emoticonPix)
            n1 = n1.nextSibling()
        emoticon.setText(textList)
        if not protocol in self.__protocolList:
            # This is the first image of a new protocol
            self.__protocolList[protocol]=[]
        if emoticon.getHtml() in map(Emoticon.getHtml, self.__protocolList[protocol]):
            # Emoticon already exists
            emoticonOld = self.__protocolList[protocol][map(Emoticon.getHtml, self.__protocolList[protocol]).index(emoticon.getHtml())]
            # We append the text to the existing emoticon
            emoticonOld.setText(emoticonOld.getText()+emoticon.getText())
        else:
            self.__protocolList[protocol].append(emoticon)

    def text2Emoticons(self, text, protocol):
        """ Converts emoticon HTML image tags into emoticon text
            Example:
             Transforms "Hello :-)" into:
            "<img src=\"/home/ociule/coipmanager/wengophone/resources/pics/emoticons/regular.png\" />"
        """
        text = self.decode(text)
        res = text
        if not protocol in self.__protocolList:
            log.error("text2Emoticons: unknown protocol "+ protocol)
            return text
        emoticonList = self.__protocolList[protocol]
        for emoticon in emoticonList:
            txtList = emoticon.getText()
            for txt in txtList:
                res = res.replace(txt, emoticon.getHtml())
                if txt!=txt.upper():
                    res = res.replace(txt.upper(), emoticon.getHtml())
        return res

    def emoticons2Text(self, text, protocol):
        """ Converts emoticon text to HTML image tags containing proper local image paths
            Thus, the emoticon graphics are displayed
            Example:
             Transforms
            "<img src=\"/home/ociule/coipmanager/wengophone/resources/pics/emoticons/regular.png\" />"
            into ":-)"
        """
        if not protocol in self.__protocolList:
            log.error("emoticons2Text: unknown protocol "+ protocol)
            return text
        emoticonList = self.__protocolList[protocol]
        for emoticon in emoticonList:
            txt = emoticon.getText()[0]
            text = text.replace(emoticon.getHtml(), self.encode(txt.upper()))
        return text

    def encode(self, text):
        """ Encodes HTML entities
        """
        ba = ""
        for i in range(0,len(text)):
            try:
                l = {'<': "&lt;",
                 '>': "&gt;",
                 '&': "&amp;",
                 '\'':"&apos;",
                 '"': "&quot;"}[text[i]]
            except KeyError:
                l = text[i]
            ba += l
        return ba

    def decode(self, text):
        """ Decodes HTML entities
        """
        entities = {"&lt;" : '<',
            "&gt;" : '>',
            "&amp;" : '&',
            "&apos;" : '\'',
            "&quot;" : '"'}
        for code, ent in entities.items():
            text = text.replace(code, ent)
        return text
