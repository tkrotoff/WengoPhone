# This script executes sip to generate .h and .cpp files for
# python binding generation.
# Sip is called from this python because we need information
# that are available in a python class.
#
# @author Philippe Bernery <philippe.bernery@wengo.com>
from PyQt4 import pyqtconfig
import os, platform, sys

USAGE="""
usage: python generateFromSipFile.py <sip_executable>
"""

platformParam = "-t "
if platform.system() == "Windows":
    platformParam += "WINDOWS_PLATFORM"
elif platform.system() == "Darwin":
    platformParam += "MACOSX_PLATFORM"
elif platform.system() == "Linux":
    platformParam += "LINUX_PLATFORM"
else:
    print "!! platform not supported"
    sys.exit(1)

def execute_sip(sip_exe):
	config = pyqtconfig.Configuration()
	commandLine = " ".join([sip_exe,
		config.pyqt_sip_flags,
		"-c", os.curdir,
		"-I", config.pyqt_sip_dir,
		"-I", os.path.join("sip_files", "std"),
		"-I", os.path.join("sip_files", "owutil"),
		"-I", os.path.join("sip_files", "qtutil"),
		"-I", os.path.join("sip_files", "coipmanager_base"),
		"-I", os.path.join("sip_files", "coipmanager"),
		"-I", os.path.join("sip_files", "coipmanager_threaded"),
		"-I", os.path.join("sip_files", "sipwrapper"),
		"-I", os.path.join("sip_files", "pixertool"),
		"-I", os.path.join("sip_files", "networkdiscovery"),
		platformParam,
		os.path.join("sip_files", "CoIpManagerBinding.sip")])
	return os.system(commandLine)

if __name__ == "__main__":
	if len(sys.argv) < 2:
		print USAGE
		sys.exit(1)
	sys.exit(execute_sip(sys.argv[1]))
