# Copy dependencies of PyCoIpManager python module
#
# Currently depdencies are:
#  - sip
#  - PyQt4
#
# @author Philippe Bernery <philippe.bernery@wengo.com>
import imp, os, shutil, sys

USAGE="""
usage: python copyBindingDependencies.py <dst>
"""

def copy_dependencies(dst):
	"""Copy sip module file and PyQt4 directory in dst"""
	# copy sip module
	fileObj, pathname, description = imp.find_module("sip")
	shutil.copy(pathname, os.path.join(dst, os.path.basename(pathname)))

	# copy PyQt4 module
	fileObj, pathname, description = imp.find_module("PyQt4")
	if os.path.exists(os.path.join(dst, "PyQt4")):
		shutil.rmtree(os.path.join(dst, "PyQt4"))
	shutil.copytree(pathname, os.path.join(dst, "PyQt4"))

	# remove pyqtconfig.py file (not needed for binding)
	os.remove(os.path.join(dst, "PyQt4", "pyqtconfig.py"))

if __name__ == "__main__":
	if len(sys.argv) < 2:
		print USAGE
		sys.exit(1)

	copy_dependencies(sys.argv[1])
