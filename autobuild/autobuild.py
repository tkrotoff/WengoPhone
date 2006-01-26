#!/usr/bin/env python2.3

import sys
import os

os.system('svn co http://dev.openwengo.com/svn/openwengo/trunk')
os.system('cd trunk; scons qtwengophone xpcomwengophone')
