import replace

"""
the below script is used to generate object_json.h and object_json.c files
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
header_fields_template = "#include<classname_json.h>"
set_case_fields_template = "\t\tcase CLASSNAME_JSON :\n\t\t{\n\t\t\tobject_p->Data = ((void*)get_classname());\n\t\t\tobject_p->Type = CLASSNAME_JSON;\n\t\t\tobject_p->Bytes = sizeof(classname);\n\t\t\tbreak;\n\t\t}"
toJson_case_fields_template = "\t\t\t\t\t\tcase CLASSNAME_JSON :\n\t\t\t\t\t\t{\n\t\t\t\t\t\t\tobject_json_result = classname_toJson(((classname*)object_p->Data));\n\t\t\t\t\t\t\tbreak;\n\t\t\t\t\t\t}"
delete_case_fields_template = "\t\t\tcase CLASSNAME_JSON :\n\t\t\t{\n\t\t\t\tdelete_classname((classname*)object_p->Data);\n\t\t\t\tbreak;\n\t\t\t}"

enum_fields = ""
header_fields = ""
set_case_fields = ""
toJson_case_fields = ""
delete_case_fields = ""

for json_object_name in command_line_args :

	json_object_name_all_caps = json_object_name.upper()
	enum_fields += (enum_fields_template.replace("classname",json_object_name).replace("CLASSNAME",json_object_name_all_caps) + "\n")
	header_fields += (header_fields_template.replace("classname",json_object_name).replace("CLASSNAME",json_object_name_all_caps) + "\n")
	set_case_fields += (set_case_fields_template.replace("classname",json_object_name).replace("CLASSNAME",json_object_name_all_caps) + "\n")
	delete_case_fields += (delete_case_fields_template.replace("classname",json_object_name).replace("CLASSNAME",json_object_name_all_caps) + "\n")
	toJson_case_fields += (toJson_case_fields_template.replace("classname",json_object_name).replace("CLASSNAME",json_object_name_all_caps) + "\n")

replace.replaceLineWithCode("../pyt/object_json.h.temp","../inc/bootstrapfiles/DTOs/object_json.h",{"//@add_headers\n":header_fields})
replace.replaceLineWithCode("../pyt/type_support.h.temp","../inc/bootstrapfiles/DTOs/type_support.h",{"//@add_enum\n":enum_fields})
replace.replaceLineWithCode("../pyt/object_json.c.temp","../src/bootstrapfiles/DTOs/object_json.c",{"//@case_handling_set)))\n":set_case_fields,"//@case_handling_delete)))\n":delete_case_fields,"//@case_handling_toJson)))\n":toJson_case_fields,"//@add_headers\n":header_fields})