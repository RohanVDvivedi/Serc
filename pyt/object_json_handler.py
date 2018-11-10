import replace

# read all commandline args
import sys
command_line_args = sys.argv.copy()[1:]

enum_fields_template = "\t,CLASSNAME_JSON"
header_fields_template = "#include<classname_json.h>"
set_case_fields_template = "\t\tcase CLASSNAME_JSON :\n\t\t{\n\t\t\tobject_p->Data = get_classname();\n\t\t\tobject_p->Type = CLASSNAME_JSON;\n\t\t\tobject_p->Bytes = sizeof(classname_json);\n\t\t\tbreak;\n\t\t}"
toJson_case_fields_template = "\t\t\t\t\t\tcase CLASSNAME_JSON :\n\t\t\t\t\t\t{\n\t\t\t\t\t\t\tobject_json_result = classname_toJson(((classname*)object_p->Data));\n\t\t\t\t\t\t\tbreak;\n\t\t\t\t\t\t}"
delete_case_fields_template = "\t\t\tcase CLASSNAME_JSON :\n\t\t\t{\n\t\t\t\tdelete_classname(object_p->Data);\n\t\t\t\tbreak;\n\t\t\t}"

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

replace.replaceLineWithCode("../pyt/object_json.h.temp","../inc/bootstrapfiles/DTOs/object_json.h",{"//@add_enum\n":enum_fields,"//@add_headers\n":header_fields})
replace.replaceLineWithCode("../pyt/object_json.c.temp","../src/bootstrapfiles/DTOs/object_json.c",{"//@case_handling_set)))\n":set_case_fields,"//@case_handling_delete)))\n":delete_case_fields,"//@case_handling_toJson)))\n":toJson_case_fields})