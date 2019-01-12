import replace

"""
the below script is used to generate combined.h, combined.c, type_support.h object_json.h and object_json.c files
which all use functions from multiple structures
for given json object name is given
it will build up enum, header includes and case statements required for building up
the complete object_json file

useage
:
python3 ./pyt/object_json_handler.py <object_name_0> <object_name_1> <object_name_2> <object_name_3> ...
put all object name in cli params

"""

# read all commandline args
import sys
command_line_args = sys.argv.copy()[1:]

enum_fields_template = "\t,CLASSNAME_JSON"
type_to_json_fields_template = "\t\t\tcase CLASSNAME_JSON :\n\t\t\t{\n\t\t\t\treturn_string = classname_toJson((classname*)data);\n\t\t\t\tbreak;\n\t\t\t}"
type_from_json_fields_template = "\t\t\tcase CLASSNAME_JSON :\n\t\t\t{\n\t\t\t\treturn_data = (void*) classname_fromJson(json_str);\n\t\t\t\tbreak;\n\t\t\t}"
type_get_fields_template = "\t\tcase CLASSNAME_JSON :\n\t\t{\n\t\t\treturn_data = (void*) get_classname();\n\t\t\tbreak;\n\t\t}"
type_del_fields_template = "\t\t\tcase CLASSNAME_JSON :\n\t\t\t{\n\t\t\t\tdelete_classname((classname*)data);\n\t\t\t\tbreak;\n\t\t\t}"
header_fields_template = "#include<classname_json.h>"


enum_fields = ""
type_to_json_fields = ""
type_from_json_fields = ""
type_get_fields = ""
type_del_fields = ""
header_fields = ""

for json_object_name in command_line_args :

	json_object_name_all_caps = json_object_name.upper()
	enum_fields += (enum_fields_template.replace("classname",json_object_name).replace("CLASSNAME",json_object_name_all_caps) + "\n")
	type_to_json_fields += (type_to_json_fields_template.replace("classname",json_object_name).replace("CLASSNAME",json_object_name_all_caps) + "\n")
	type_from_json_fields += (type_from_json_fields_template.replace("classname",json_object_name).replace("CLASSNAME",json_object_name_all_caps) + "\n")
	type_get_fields += (type_get_fields_template.replace("classname",json_object_name).replace("CLASSNAME",json_object_name_all_caps) + "\n")
	type_del_fields += (type_del_fields_template.replace("classname",json_object_name).replace("CLASSNAME",json_object_name_all_caps) + "\n")
	header_fields += (header_fields_template.replace("classname",json_object_name).replace("CLASSNAME",json_object_name_all_caps) + "\n")
	


replace.replaceLineWithCode("../pyt/type_support.h.temp","../inc/bootstrapfiles/DTOs/type_support.h",{"//@add_enum\n":enum_fields,"//@headers_all_json)))\n":header_fields})
replace.replaceLineWithCode("../pyt/combined.h.temp","../inc/bootstrapfiles/DTOs/combined.h",{"//@headers_all_json)))\n":header_fields})
replace.replaceLineWithCode("../pyt/combined.c.temp","../src/bootstrapfiles/DTOs/combined.c",{"//@add_case_for_objects for to_json)))\n":type_to_json_fields,"//@add_case_for_objects for from_json)))\n":type_from_json_fields,"//@add_case_for_objects for get)))\n":type_get_fields,"//@add_case_for_objects for del)))\n":type_del_fields})

