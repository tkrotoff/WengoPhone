#!/usr/bin/env python
#
# Converts a .cpp C++ file with WengoPhone coding style into a .py
#
# @author Ovidiu Ciule <ovidiu.ciule@wengo.com>

import os, re, sys

USAGE="""
Usage: CppToPyConverter.py <cpp_file> [<py_file>]
If no output file is given, the name is infered and the file is created in the current directory.
"""

def bloc_open(content):
    """
    Transform bloc opens from C++ style ' {' into python style ':'
    """
    if content.rstrip().endswith(" {"):
        # End ' {' found
        return re.sub(r'\s*?{$', r':', content.rstrip())
    else:
        return content

def bloc_end(content):
    """
    Removes C++ bloc ending '}'
    """
    return re.sub(r'}', r'', content)

def remove_semicolon(content):
    """
    Remove semi-colons
    """
    rexp = re.compile(r';$')
    return rexp.sub(r'', content.rstrip())

def transform_module_delim(content):
    """
    e.g: 'Toto::Tata' will become 'Toto.Tata'
    """
    rexp = re.compile(r'::')
    return rexp.sub(r'.', content)

def this_to_self(content):
    """
    e.g.: 'this' into 'self'
    """
    rexp = re.compile(r"this->")
    return rexp.sub(r'self.', content)

def pointer_to_point(content):
    """
    e.g.: '->' into '.'
    """
    rexp = re.compile(r"->")
    return rexp.sub(r'.', content)

def tab_to_space(content):
    """
    Transforms tab into 4 spaces
    """
    rexp = re.compile(r"""\t""")
    return rexp.sub(r'    ', content)

def transform_license_comment(lines):
    """
    Transforms C++ comment into Python style
    Pre-condition: lines is a list of strings
    First multiline comment must be license
    """
    license = False
    for line in lines:
        if line.strip().find("/*")!=-1:
            license = True
        if line.strip().find("*/")!=-1:
            license = False
            lines[lines.index(line)]=transform_comment(line)
        if license:
            lines[lines.index(line)]=transform_comment(line)
    return lines

def transform_comment(content):
    """
    '/*', ' *' or '*/' into '#'
    """
    content = content.replace("/*", "#", 1)
    content = content.replace("*/", "#", 1)
    content = content.replace(" *", "#", 1)
    return content.strip()

def transform_bool(content):
    """
    true->True
    false->False
    """
    content = content.replace("true", "True")
    content = content.replace("false", "False", 1)
    return content

def remove_empty_lines(text):
    """
    Removes empty lines from multi-line text
    """
    out = []
    if (not isinstance(text, list)):
        text = text.split("\n")
    for line in text:
        if (len(line.strip())!=0):
            out.append(line.rstrip()+"\n")
    return "".join(out)

def py_generator(cpp_file_name, py_file_name = None):
    inFile = open(cpp_file_name, "r")
    if inFile == None:
        print " Error: "+cpp_file_name+" could not be opened"
    try:
        lines = inFile.readlines()
    finally:
        inFile.close()
    print " "+ str(len(lines))+" lines read"
    result = ""
    output = []

    lines = transform_license_comment(lines)

    for line in lines:
        line = bloc_open(line)
        line = bloc_end(line)
        line = remove_semicolon(line)
        line = transform_module_delim(line)
        line = this_to_self(line)
        line = pointer_to_point(line)
        line = tab_to_space(line)
        line = transform_bool(line)

        if (len(line.strip())!=0):
            output.append(line.rstrip()+"\n")

    outText = "".join(output)

    #outText = remove_empty_lines(outText)
    """ Not necessary, it seems """

    outFile = open((py_file_name, os.path.splitext(os.path.basename(inFile.name))[0]+".py")[py_file_name == None], "w")
    outFile.write(outText)
"""
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print USAGE
        sys.exit(1)

    if len(sys.argv)==3:
        py_generator(sys.argv[1], sys.argv[2])
    else:
        py_generator(sys.argv[1])
"""
py_generator("/home/ociule/coipmanager/pywengophone/src/util/WidgetRatioEnforcer.cpp")
