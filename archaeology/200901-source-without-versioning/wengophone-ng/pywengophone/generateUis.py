#!/usr/bin/python
# 
# .ui compiler script for PyWengoPhone
#
# @author Ovidiu Ciule <ovidiu.ciule@wengo.com>
# @author Philippe Bernery
import glob, os, shutil

def filter_dir(a, dirname, names):
    """
    Calls process_files only on appropriate files. (not . directory, etc...)
    """
    if os.path.basename(dirname).startswith("."):
        del(names[:])
        return
	for f in glob.glob(dirname + "/*_ui.py?"):
		shutil.remove(f)
    process_files(dirname, names)

def process_files(dirname, names):
    """
    Gets all .ui files of dirname and calls process_file on these files.
    """
    print "** in : " + dirname
    for name in names:
        if not name.startswith(".") and \
            not os.path.isdir(dirname + "/" + name) \
            and name.endswith(".ui"):
            process_file(os.path.join(dirname, name), dirname)

def process_file(filename, outdir):
    """
    Calls pyuic4 on filename and put the result in a file in outdir.
    """
    command = "pyuic4 \"" + filename + "\" -o \"" + os.path.join(outdir, process_name(filename)) + "\"" 
    print("** " + command)
    os.system(command)
    if filename.rstrip().endswith("ContactWidget.ui") or \
        filename.rstrip().endswith("IMContactManager.ui") or \
        filename.rstrip().endswith("ProfileDetails.ui") or \
        filename.rstrip().endswith("ChatTabWidget.ui") or \
        filename.rstrip().endswith("WengoPhoneWindow.ui") or \
        filename.rstrip().endswith("FileTransferItem.ui") or \
        filename.rstrip().endswith("ChatMainWindow.ui"):
        # Need to fix broken import statement
        fix_import(os.path.join(os.getcwd(),outdir,process_name(filename)))
 
def process_name(name):
    """
    Returns the name of the given file with appended _ui.py and path removed.
    e.g: ../wengophone/src/presentation/qt/contactlist/ContactList.ui => ContactList_ui.py
    """
    return os.path.basename(os.path.splitext(name)[0]) + "_ui.py"

def fix_import(filename):
    """
    Fixes broken import lines that include paths.
    Example:
    Transforms 'import ../../resources/wengophone_rc'
    into
    'import wengophon_rc'
    """
    file = open(filename, "r")
    try:
        lines = file.readlines()
    finally:
        file.close()
    output = []

    for line in lines:
        if line.lstrip().startswith("import "):
            line = fix_import_line(line)
            print "**** "+filename+" fixed import: "+ line
                
        output.append(line.rstrip()+os.linesep)

        outText = "".join(output)

    file.close()
    file = open(filename, "w")
    file.write(outText)

def fix_import_line(line):
    if line.lstrip().startswith("import "):
        return "import "+ get_import_target(line)+os.linesep
    else:
        return line

def get_import_target(line):
    line = line.strip()
    if line.startswith("import "):
        line = line[7:]
        if line.rfind(os.sep)>=0:
            line = line[line.rfind(os.sep)+1:]
    return line
     
if __name__ == "__main__":
    print "** walk current directory..."
    os.path.walk(os.getcwd(), filter_dir, None)

    print "** generate *_ui.py for listed files..."
    # Generate _ui.py files for listed files.
    # Path to files are given from root repository directory
    fileDict = {
        "../wengophone/src/presentation/qt/contactlist/ContactWidget.ui" : "contact/",
        "../wengophone/src/presentation/qt/imcontact/IMContactManager.ui" : "contact/",
        "../wengophone/src/presentation/qt/profile/ProfileDetails.ui" : "userprofile/",
        "../wengophone/src/presentation/qt/chat/ChatMainWindow.ui" : "chat/",
        "../wengophone/src/presentation/qt/filetransfer/FileTransferDialog.ui" : "filetransfer/",
        "../wengophone/src/presentation/qt/filetransfer/AcceptDialog.ui" : "filetransfer/",
        "../wengophone/src/presentation/qt/filetransfer/FileTransferItem.ui" : "filetransfer/",
        "../wengophone/src/presentation/qt/WengoPhoneWindow.ui" : ".",
    }

    for f in fileDict.items():
        process_file(f[0], os.path.join("src", f[1]))

    # Process .qrc file
    os.system("pyrcc4 ../wengophone/resources/wengophone.qrc -o src/wengophone_rc.py")
