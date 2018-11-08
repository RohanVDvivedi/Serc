import re
import replace
import datatype_handler


'''
below function replaces all \n, \t, \r to just spaces individually
then strips out white spaces from front and back
then replaces all multiple spaces to single space
'''
def replace_white_space_with_space(string) :
	return (re.sub(' +',' ',string.replace('\n',' ').replace('\t',' ').replace('\r',' ').strip())).strip()







'''
below function takes json object name as the parameter
it opens a file reads all of it identifying the structure/ json object definition
it returns a string between { and } that contains all fields of the structure
'''
def read_file_extract_string_consisting_fields(json_object_name) :

	object_declaration_fileName = "../inc/DTOs_declarations/" + json_object_name + "_json.h"
	object_declaration_file = open(replace.dir_path + '/' + object_declaration_fileName,'r')

	inside_object_state = -1

	field_string = ""

	'''
	the below piece of loop will read the c header to find all the text words between '{\n' and '};\n'
	and put them in field_string
	'''
	for line in object_declaration_file :
		line = replace_white_space_with_space(line).strip()
		if (("struct " + json_object_name) in line) and inside_object_state == -1 :
			inside_object_state = 0
		if ("{" in line) and inside_object_state == 0 :
			field_string += line.rsplit("{",1)[1]
			if("}" in field_string) :
				field_string = field_string.rsplit("}",1)[0]
				inside_object_state = 2
				break
			inside_object_state = 1
			continue
		if ("}" in line) and inside_object_state == 1 :
			field_string += line.rsplit("}",1)[0]
			inside_object_state = 2
			break
		if (";" in line) and inside_object_state == 0 :
			inside_object_state = -1
		if inside_object_state == 1:
			field_string += line;
		if inside_object_state == 2 :
			break

	object_declaration_file.close()

	return field_string



'''
below function takes field_strings as parameter, which is an array of
strings each of the string is an compound of datatype and variable
we isolate them and return array of string pairs, each of its
first element is datatype in string as mentioned in code and 
second string is the variable name as used in the code
'''
'''
fields is a 2d array that holds division when datatype ends and the field name starts
all white spaces will be converted to space and trimmed from both sides
'''
def isolate_datatype_variable(field_strings) :
	fields = []

	for fieldStr in field_strings :
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
		fields += [[replace_white_space_with_space(fieldi[0]), replace_white_space_with_space(fieldi[1])]]

	return fields

def identify_data_type(datatype_string) :
	for key,list_data_type_string in datatype_handler.dataTypeStrings.items() :
		if datatype_string in list_data_type_string :
			return key
	return datatype_handler.DataType.OTHER

def identify_data_type_flexible(datatype_string) :
	for key,list_data_type_string in datatype_handler.dataTypeStrings_flexible.items() :
		for data_type_string in list_data_type_string :
			if data_type_string in datatype_string :
				return key
	return datatype_handler.DataType.OTHER
