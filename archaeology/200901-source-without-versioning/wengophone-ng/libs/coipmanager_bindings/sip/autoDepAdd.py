import os

err = True
undef_err = True
undef_cl = ""
old_undef_cl = undef_cl
while undef_err:
	undef_err = False
	err = False
	old_undef_cl = undef_cl
	old_wd = os.getcwd()
	os.chdir("/home/ociule/coipmanager/build")
	os.system("make PyCoIpManager 2> makeOut.txt")
	file = open("makeOut.txt", "r")
	lines = file.readlines()
	for line in lines:
		if line.find("Error")!=-1:
			err = True
		if line.find("defined")!=-1:
			undef_err = True
			line = line[5:]
			ix_end_undef = line.index(" ")
			undef_cl = line[0:ix_end_undef]
			if undef_cl.find("::")!=-1:
				ix_member = undef_cl.find("::")
				undef_cl = undef_cl[0:ix_member]
	if undef_err:
		print "\n\n\n*********************\n>>>>>>> Undefined: " + undef_cl
		coipbind = open("/home/ociule/coipmanager/libs/coipmanager_bindings/sip/sip_files/CoIpManagerBinding.sip", "a")
		coipbind.write("\n%Include "+undef_cl+".sip")
		coipbind.close()
		print "\n>>>>>>> Wrote: " + undef_cl
			
	if err and not undef_err:
		print ">> ********* Unknown error, please debug *************************"
	if (old_undef_cl==undef_cl):
		print ">> ********* Same undef, please debug *************************"
		break
	file.close()
			
os.chdir(old_wd)
file.close()
