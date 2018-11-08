import replace
import datatype_handler
import jsonner_parse_helper as jph


json_object_name = "MyObject"


field_string = jph.read_file_extract_string_consisting_fields(json_object_name)


'''
since every struct field declarastio0n ends with ; in c
we split all of them at ; and get individual fieldStrings as a list
'''
field_strings = field_string.strip().split(';')


'''
fields is a 2d array that holds division when datatype ends and the field name starts
all white spaces will be converted to space and trimmed from both sides
'''
fields = jph.isolate_datatype_variable(field_strings)


'''
in the below loop we just transform DataType representations 
and identify them in specific enum as a normalizing step
this is required because 'int' and 'signed int' are same dataTypes
'''
for fieldi in fields :
	data_type = jph.identify_data_type_flexible(fieldi[0])
	fieldi += [fieldi[0].count("*")]
	if data_type == datatype_handler.DataType.OTHER:
		fieldi += [fieldi[0].replace("*","").replace(" ","")]
	fieldi[0] = data_type
'''
if DataType is not array or other
ith element is 
feild[i] = [ DataType, variable_name_in_string, levels of pointer to reach data ]
else
feild[i] = [ DataType, variable_name_in_string, levels_of_pointer_to_reach_data, other_data_type_name_in_string ]
'''

"""
over here after completing the above loop we now have fields as a dictionary 
with key as a DataType enum and value as variable name
"""
function_string = datatype_handler.to_json_function_creator(json_object_name,fields)

replace.replaceLineWithCode("../pyt/DTOs_method.temp","../src/DTOs_methods/" + json_object_name + "_json.c",{"@DTO_declaration file include\n":"#include<" + json_object_name + "_json.h>\n#include<jsonstringhandler.h>\n#include<stdlib.h>\n#include<stdio.h>\n","@add_methods\n":function_string})


