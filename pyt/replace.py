import os;

dir_path = os.path.dirname(os.path.realpath(__file__))

# the below function takes a template filename and code file name to edit
# it picks up each c one line type comment if found it replaces it with the 
# given code in the hash passed
def replaceLineWithCode(templateFileName,codeFileName,compareCodeHash):
	global dir_path
	Templatefile = open(dir_path + "/" + templateFileName,"r");
	codefile = open(dir_path + "/" + codeFileName,"w");

	#replace case string for the comment "//@switchcase\n" in c fil
	for line in Templatefile:
		test = line
		if test in compareCodeHash:
			codefile.write(compareCodeHash[line])
		else:
			codefile.write(line)

	Templatefile.close();
	codefile.close();