import re
import replace
import datatype_handler

'''
below function replaces all \n, \t, \r to just spaces individually
then strips out white spaces from front and back
then replaces all multiple spaces to single space
'''
def replaceWhiteSpaceWithSpace(string) :
	return (re.sub(' +',' ',string.replace('\n',' ').replace('\t',' ').replace('\r',' ').strip())).strip()

json_object_name = "MyObject"


object_declaration_fileName = "../inc/DTOs_declarations/" + json_object_name + "_json.h"
object_declaration_file = open(replace.dir_path + '/' + object_declaration_fileName,'r')

insideobject = -1

fieldString = ""

'''
the below piece of loop will read the c header to find all the text words between '{\n' and '};\n'
and put them in fieldString
'''
for line in object_declaration_file :
	line = replaceWhiteSpaceWithSpace(line).strip()
	if (("struct " + json_object_name) in line) and (("typedef struct " + json_object_name) not in line) and (";" not in line) and insideobject == -1 :
		insideobject = 0
	if ("{" in line) and insideobject == 0 :
		fieldString += line.rsplit("{",1)[1]
		if("}" in fieldString) :
			fieldString = fieldString.rsplit("}",1)[0]
			break
		insideobject = 1
		continue
	if ("}" in line) and insideobject == 1 :
		fieldString += line.rsplit("}",1)[0]
		insideobject = 2
		break
	if (";" in line) and insideobject == 0 :
		insideobject = -1
	if insideobject == 1:
		fieldString += line;
	if insideobject == 2 :
		break


object_declaration_file.close()


'''
since every struct field declarastio0n ends with ; in c
we split all of them at ; and get individual fieldStrings as a list
'''
fieldString = fieldString.strip().split(';')

'''
fields is a 2d array that holds division when datatype ends and the field name starts
all white spaces will be converted to space and trimmed from both sides
'''
fields = []

for fieldStr in fieldString :
	if fieldStr == '' :
		continue
	if "*" in fieldStr :
		fieldi = fieldStr.strip().rsplit("*",1)
		if (len(fieldi)>1) and (" " in fieldi[1]) :
			fieldi = fieldStr.strip().rsplit(" ",1)
		else :
			fieldi[0] += "*"
	else :
		fieldi = fieldStr.strip().rsplit(" ",1)
	fields += [[replaceWhiteSpaceWithSpace(fieldi[0]), replaceWhiteSpaceWithSpace(fieldi[1])]]

'''
in the below loop we just transform DataType representations 
and identify them in specific enum as a normalizing step
this is required because 'int' and 'signed int' are same dataTypes
'''
for fieldi in fields :
	found = 0
	for key,list_data_type_string in datatype_handler.dataTypeStrings.items() :
		if fieldi[0] in list_data_type_string :
			fieldi[0] = key
			found = 1
	if found == 0:
		temp = fieldi[0]
		fieldi[0] = datatype_handler.DataType.OTHER
		fieldi += [temp.replace("*","").replace(" ","")]

"""
over here after completing the above loop we now have fields as a dictionary 
with key as a DataType enum and value as variable name
"""

function_string = datatype_handler.to_json_function_creator(json_object_name,fields)

replace.replaceLineWithCode("../pyt/DTOs_method.temp","../src/DTOs_methods/" + json_object_name + "_json.c",{"@DTO_declaration file include\n":"#include<" + json_object_name + "_json.h>\n#include<jsonstringhandler.h>\n#include<stdlib.h>\n#include<stdio.h>\n","@add_methods\n":function_string})


