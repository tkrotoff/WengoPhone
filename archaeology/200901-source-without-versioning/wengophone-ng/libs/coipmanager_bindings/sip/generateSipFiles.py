import os, SipGenerator, fileList

INPUT_DIR = "/home/ociule/coipmanager"
OUTPUT_DIR = "/home/ociule/coipmanager/libs/coipmanager_bindings/sip/sip_files"

def execTasks(tasks):
	for task in tasks:
		execTask(task)

def execTask(task):
	if (not os.path.exists(OUTPUT_DIR +"/"+ task[0])):
		os.mkdir(OUTPUT_DIR +"/"+ task[0])
	for file in task[1:]:
		SipGenerator.sip_generator(os.path.join(INPUT_DIR,file), OUTPUT_DIR+"/"+task[0]+"/"+os.path.splitext(os.path.basename(file))[0]+".sip")

"""
Task format:
A task is just a list of strings. The first string is the output folder, where 
all the tasks sip files will be written. The output folder path is relative to 
the OUTPUT_DIR. Next, we have the header files for which SIP files 
are to be created.
"""

execTasks(fileList.tasks)
