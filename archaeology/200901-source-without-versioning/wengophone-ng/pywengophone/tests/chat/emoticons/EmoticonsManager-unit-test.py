"""
    EmoticonsManager unit tests
    @author Ovidiu Ciule
"""
from chat.emoticons.EmoticonsManager import *
from config import Config
from PyQt4.QtGui import QApplication
import unittest, sys

class TestEmoticonsManagerClass(unittest.TestCase):
    def setUp(self):
        # Need to built a QApplication
        self.__em = EmoticonsManager.getInstance()
        self.__protocols = ["msn", "yahoo", "default"]

    def testEncodeDecodeEmoticonsText(self):
        for protocol in self.__protocols:
            for emoticon in self.__em.getEmoticonList(protocol):
                input = emoticon.getText()[0]
                output = self.__em.decode(self.__em.encode(input))
                expected = emoticon.getText()[0]
                self.failUnlessEqual(output, expected,
                    "failed EncodeDecodeEmoticonText input: text='"
                    + input +"'\n output: expected='"
                    + expected +"'\n obtained='"+
                    output + "'\n protocol="+protocol
                    +"\n emoticon.getText()="+str(emoticon.getText()))

    def testIndividualEmoticons2Text(self):
        for protocol in self.__protocols:
            for emoticon in self.__em.getEmoticonList(protocol):
                input = emoticon.getHtml()
                output = self.__em.emoticons2Text(input, protocol)
                expected = self.__em.encode(emoticon.getText()[0].upper())
                self.failUnlessEqual(output, expected,
                    "failed Emoticon2Text input: html='"
                    + input +"'\n output: expected='"
                    + expected +"'\n obtained='"+
                    output + "'\n protocol="+protocol
                    +"\n emoticon.getText()="+str(emoticon.getText()))

    def testIndividualText2Emoticons(self):
        for protocol in self.__protocols:
            for emoticon in self.__em.getEmoticonList(protocol):
                input = emoticon.getText()[0].upper()
                output = self.__em.text2Emoticons(input, protocol)
                expected = emoticon.getHtml()
                self.failUnlessEqual(output, expected,
                    "failed Text2Emoticons input: text='"
                    + input +"'\n output: expected='"
                    + expected +"'\n obtained='"+
                    output + "'\n protocol="+protocol
                    +"\n emoticon.getText()="+str(emoticon.getText()))

    def textComplexText2Emoticons(self): pass

if __name__ == '__main__':
    logging.basicConfig()
    log = logging.getLogger("PyWengoPhone")
    log.setLevel(logging.DEBUG)
    Config.getInstance().setResourcesPath("/home/ociule/coipmanager/wengophone/resources")
    app = QApplication(sys.argv)
    unittest.main()
