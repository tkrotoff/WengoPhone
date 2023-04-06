"""
    EmoticonsManager unit tests

    @author Ovidiu Ciule
"""
from chat.emoticons.EmoticonsManager import *
from config import Config
from PyQt4.QtGui import QApplication
import sys

if __name__ == '__main__':
    logging.basicConfig()
    log = logging.getLogger("PyWengoPhone")
    log.setLevel(logging.DEBUG)
    app = QApplication(sys.argv)
    Config.getInstance().setResourcesPath("/home/ociule/coipmanager/wengophone/resources")
    __em = EmoticonsManager()

    print __em.text2Emoticons(":-)", "all")
