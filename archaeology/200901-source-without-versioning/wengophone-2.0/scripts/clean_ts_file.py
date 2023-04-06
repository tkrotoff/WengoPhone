# !/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Clean TS files (Qt translation files) by removing all 'obsolete' nodes.
#
# @author Philippe Bernery
#

import codecs, sys, xml.dom.minidom
from xml.dom.minidom import Node

# Usage
def usage():
	print 'Usage: python clean_ts_file.py <file to clean>'
	print
	sys.exit(-1)
##

# Parsing command line parameters
if (len(sys.argv) < 2):
	usage()

fileToClean = sys.argv[1]
##

# Removing all obsolete nodes
doc = xml.dom.minidom.parse(fileToClean)
for node in doc.getElementsByTagName('translation'):
	typeAttr = node.getAttribute('type')
	if (typeAttr == 'obsolete'):
		parentNode = node.parentNode
		parentNode.parentNode.removeChild(parentNode)
##

# Saving the new file
newFileName = fileToClean + '_clean'
newFile = codecs.open(newFileName, 'w', 'utf-8')
doc.writexml(newFile, encoding='utf-8')
newFile.close()
##
