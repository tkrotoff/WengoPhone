#!/usr/bin/env python
#
# Generates .sip file from a C++ file with WengoPhone coding style
#
#
# @author Philippe Bernery <philippe.bernery@wengo.com>
# @author Ovidiu Ciule <ovidiu.ciule@wengo.com>

import os, re, sys

USAGE="""
Usage: SipGenerator.py <header_file> [<sip_file>]
If no output file is given, the name is infered and the file is created in the current directory.
"""

def remove_sharp_line(content):
	"""
	Remove lines beginning with a #
	"""
	return re.sub(r'^#.*$', r'', content)

def remove_class_line(content):
	"""
	Remove forward class declarations: [friend] class CLASSNAME; lines beginning with class and ending with ;
	"""
	rexp = re.compile(r'^.*?class [^;]*;[^\n]*$')
	return rexp.sub(r'', content)

def remove_inheritance_type(content):
	"""
	Removes inheritance type if present.
	e.g: 'class Toto : public Tata' will become 'class Toto : Tata'
	"""
	rexp = re.compile(r'(?<=:).*?(public|private|protected)')
	return rexp.sub(r'', content)

def remove_comment_from_line(content):
	"""
	Removes C++ // comments
	"""
	rexp = re.compile(r"""//.*""")
	return rexp.sub(r'', content)

def remove_multiline_comments(content):
	"""
	Removes C++ /* */ multiline comments
	"""
	rexp = re.compile(r"""/\*(.|\n)*?\*/""")
	return rexp.sub(r'', content)

def remove_q_object_line(content):
	"""
	Removes Q_OBJECT moc directive 
	"""
	rexp = re.compile(r""".*?Q_OBJECT.*?""")
	return rexp.sub(r'', content)

def remove_api_directive(content):
	"""
	Removes *_API
	"""
        rexp = re.compile(r"""\s\S*?_API""")
        return rexp.sub(r'', content)

def replace_q_signals_line(content):
	"""
	Reformats Q_SIGNALS moc directive 
	"""
	rexp = re.compile(r""".*?Q_SIGNALS.*?""")
	return rexp.sub(r'signals', content)

def replace_q_slots_line(content):
	"""
	Reformats Q_SLOTS moc directive 
	"""
	rexp = re.compile(r"""\S*?Q_SLOTS.*?""")
	return rexp.sub(r'slots', content)

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

def add_type_header_code_line(content, filename):
	"""
	Adds %TypeHeaderCode, the #include directives and closes with %End,
	after the "class CLASSNAME [: PARENT] {" line
	"""
	if ((content.count("class") == 1) and (content.count("{") == 1)): 
		content = content + "\n%TypeHeaderCode\n"
		content = content + "#include <" + os.path.splitext(get_original_header_include_path(filename))[0] + ".h>\n%End\n"
	return content

def get_original_header_include_path(filename):
	"""
	Gets the appropriate path for the #include directive from the file name.
	Example:
	/home/ociule/coipmanager/libs/coipmanager_threaded/include/coipmanager_threaded/datamanager/TUserProfileManager.h
	becomes
	coipmanager_threaded/datamanager/TUserProfileManager.h
	Pre-condition: include/ in original path, otherwise does nothing
	"""
	if (filename.count("include") == 1):
		filename = filename.partition("include/")[2]
	if (filename.count("owutil") == 1):
		filename = filename.partition("owutil/")[2]
	return filename

def get_first_parent(line):
	"""
	Returns the first parent, if any
	"""
	parent = re.search(r"""(?<=\s:\s)\S*""", line).group()
	if parent.find("std::")!=-1:
		parent = ""
	if parent.endswith(","):
		parent = parent[:-1]
	return parent

def remove_multiple_parents(line):
	"""
	Removes all but the first parent of a class, because SIP does not support multiple inheritance.
	Pre-conditon: inheritance qualifier (public|private|protected) has already been removed.
	If there is no parent, does nothing
	"""
	if ((line.count("class") == 1) and (line.count(":")>=1) and (line.count("{") == 1)):
		parent = get_first_parent(line)
		if len(parent)>0:
			line = line.partition(" : ")[0]	+ " : " + parent + " {"
		else:
			line = line.partition(" : ")[0]	+ " {"
	return line



def remove_not_public_variables(lines):
	"""
	Removes all private or protected variables, if present, because SIP 
	only supports public variables.
	Pre-condition: lines is a list of strings
	"""
	not_public = False
	for line in lines:
		if (line.strip().find("private:")!=-1 or line.strip().find("protected:")!=-1):
			not_public = True
		if (line.strip().find("public:")!=-1 or line.strip().startswith("}")):
			not_public = False
		if (not_public and isavariable(line)):
			lines[lines.index(line)]=""
	return lines

def isavariable(line):
	"""
	Detects variable definitions;
	Pre-condition: the line contains a class member: either an attribute or a method
	"""
	is_var = (line.find("(")==-1) and (line.find(")")==-1) and (line.find(";")>=0) 
	is_var &= (line.find("protected:")==-1) and (line.find("public:")==-1) and (line.find("private:")==-1)
	is_var &= (len(line.strip())>0)
	is_var &= (not line.strip().startswith("/**")) and (not line.strip().startswith("/*"))
	is_var &= (not line.strip().startswith("*")) and (not line.strip().startswith("*/"))
	
	return is_var	

def remove_operator_eq_line(line):
	"""
	Removes the definition of the = operator, because SIP 4.5 does not support
	it.
	"""
	if (line.strip().find("operator=")!=-1) or (line.strip().find("operator =")!=-1):
		line = ""
	return line

def comment_extern_c_line(line):
	"""
	Comments any extern "C" directives. Does not delete them, because it 
	would make any necessary debugging harder
	"""
	if line.find("extern \"C\"")!=-1:
		linel = line.partition("extern \"C\"")
		line = linel[0] + "/*" + linel[1] + "*/" + linel[2]
	return line
				
def sip_generator(h_file_name, sip_file_name = None):
	"""
	Produces a .sip file from a .h file with WengoPhone coding-style.
	"""
	inFile = open(h_file_name, "r")
	try:
		lines = inFile.readlines()
	finally:
		inFile.close()
	result = ""
	output = []

	lines = remove_not_public_variables(lines)

	for line in lines:
		line = remove_sharp_line(line)
		line = remove_class_line(line)
		line = remove_inheritance_type(line)
		line = remove_comment_from_line(line)
		line = remove_q_object_line(line)
		line = replace_q_signals_line(line)
		line = replace_q_slots_line(line)
		line = remove_multiple_parents(line)
		line = remove_operator_eq_line(line)
		line = add_type_header_code_line(line, inFile.name)
		line = comment_extern_c_line(line)
		
		if (len(line.strip())!=0):
			output.append(line.rstrip()+"\n")
	
	outText = "".join(output)
	
	outText = remove_multiline_comments(outText)
	outText = remove_api_directive(outText)
	outText = remove_empty_lines(outText)
	""" Not necessary, it seems """
	#outText = "%Import QtCoremod.sip\n"+outText
	
	outFile = open((sip_file_name, os.path.splitext(os.path.basename(inFile.name))[0]+".sip")[sip_file_name == None], "w")
	outFile.write(outText)
	
if __name__ == "__main__":
	if len(sys.argv) < 2:
		print USAGE
		sys.exit(1)

	if len(sys.argv)==3:
		sip_generator(sys.argv[1], sys.argv[2])
	else:
		sip_generator(sys.argv[1])
#sip_generator("/home/ociule/coipmanager/libs/owutil/util/Date.h")
#print isavariable("*")
