import enum;

class DataType(enum.IntEnum) :
	SIGNED_INT = 0
	UNSIGNED_INT = 1
	SIGNED_LONG_INT = 2
	UNSIGNED_LONG_INT = 3
	SIGNED_LONG_LONG_INT = 4
	UNSIGNED_LONG_LONG_INT = 5
	CHARACTER = 6
	UNSIGNED_CHARACTER = 7
	STRING = 8
	BOOLEAN = 9
	FLOAT = 10
	DOUBLE = 11
	OTHER = 12
	ARRAY = 13

dataTypeStrings = {
	DataType.UNSIGNED_LONG_LONG_INT:['unsigned long long int','unsigned long long'],
	DataType.SIGNED_LONG_LONG_INT:['long long int','long long','signed long long int'],
	DataType.UNSIGNED_LONG_INT:['unsigned long int','unsigned long','unsigned long int'],
	DataType.SIGNED_LONG_INT:['long int','long','signed long int'],
	DataType.UNSIGNED_INT:['unsigned int'],
	DataType.SIGNED_INT:['int','signed int'],
	DataType.CHARACTER:['char','signed char'],
	DataType.UNSIGNED_CHARACTER:['unsigned char'],
	DataType.STRING:['char*','char *'],
	DataType.BOOLEAN:['bool'],
	DataType.FLOAT:['float'],
	DataType.DOUBLE:['double']
}

dataTypeStrings_flexible = {
	DataType.UNSIGNED_LONG_LONG_INT:['unsigned long long'],
	DataType.SIGNED_LONG_LONG_INT:['long long'],
	DataType.UNSIGNED_LONG_INT:['unsigned long'],
	DataType.SIGNED_LONG_INT:['long'],
	DataType.UNSIGNED_INT:['unsigned int'],
	DataType.SIGNED_INT:['int'],

	DataType.UNSIGNED_CHARACTER:['unsigned char'],
	DataType.STRING:['char*','char *'],
	DataType.CHARACTER:['char'],

	DataType.BOOLEAN:['bool'],

	DataType.FLOAT:['float'],
	DataType.DOUBLE:['double'],

	DataType.ARRAY:['array_json']
}

dataTypeFormatSpecifierStrings = {
	DataType.SIGNED_INT:'%d',
	DataType.UNSIGNED_INT:'%u',
	DataType.SIGNED_LONG_INT:'%ld',
	DataType.UNSIGNED_LONG_INT:'%lu',
	DataType.SIGNED_LONG_LONG_INT:'%lld',
	DataType.UNSIGNED_LONG_LONG_INT:'%llu',
	DataType.CHARACTER:'%c',
	DataType.UNSIGNED_CHARACTER:'%c',
	DataType.STRING:'%s',
	DataType.FLOAT:'%f',
	DataType.DOUBLE:'%lf'
}

def forJson_forNumber(fieldi) :
	pointer_variable = "(object->" + fieldi[1] + ")"
	code = ""
	code  = ""
	code += "\n\t\tif( value != NULL && value->type == NUMBER_JSON )"
	code += "\n\t\t{"
	code += "\n\t\t\tchar prev_char = (*(value->end_index + 1));"
	code += "\n\t\t\t(*(value->end_index + 1)) = \'\\0\';"
	code += "\n\t\t\t"
	code += "\n\t\t\tsscanf(value->start_index,\"" + dataTypeFormatSpecifierStrings[fieldi[0]] + "\",(&" + pointer_variable + "));";
	code += "\n\t\t\t"
	code += "\n\t\t\t(*(value->end_index + 1)) = prev_char;"
	code += "\n\t\t}"
	return code

def toJson_forNumber(fieldi) :
	code  = ""
	pointer_variable = "(object->" + fieldi[1] + ")"
	tab_if_required = ""
	if fieldi[2] > 0 :
		tab_if_required = "\t"
		code += "\n\tif( "
		for i in range(fieldi[2]) :
			code += "( ( " + pointer_variable + " ) != NULL )"
			if i != fieldi[2]-1 : 
				code += " && "
			i -= 1
			pointer_variable = "*" + pointer_variable
		code += " )"
		code += "\n\t{"
	code += "\n\t" + tab_if_required + "sprintf(number,\"" + dataTypeFormatSpecifierStrings[fieldi[0]] + ",\", (" + pointer_variable + ") );"
	if fieldi[2] > 0 :
		code += "\n\t}"
		code += "\n\telse"
		code += "\n\t{"
		code += "\n\t\tsprintf(number,\"null,\");"
		code += "\n\t}"
	code += "\n\taddToJsonString(JS,\"\\\"" + fieldi[1] + "\\\":\");"
	code += "\n\taddToJsonString(JS,number);"
	code += "\n"
	return code

def forJson_forString(fieldi) :
	code  = ""
	code += "\n\t\tif( value != NULL && value->type == STRING_JSON )"
	code += "\n\t\t{"
	code += "\n\t\t\t"
	code += "\n\t\t}"
	return code

def toJson_forString(fieldi) :
	code  = ""
	code += "\n\taddToJsonString(JS,\"\\\"" + fieldi[1] + "\\\":\");"
	pointer_variable = "(object->" + fieldi[1] + ")"
	tab_if_required = ""
	if fieldi[2] > 0 :
		tab_if_required = "\t"
		code += "\n\tif( "
		for i in range(fieldi[2]) :
			code += "( ( " + pointer_variable + " ) != NULL )"
			if i != fieldi[2]-1 : 
				code += " && "
			i -= 1
			pointer_variable = "*" + pointer_variable
		code += " )"
		code += "\n\t{"
	code += "\n\t" + tab_if_required + "addToJsonString(JS,\"\\\"\");"
	code += "\n\t" + tab_if_required + "addToJsonString(JS, (" + pointer_variable[1:] + ")" + " );"
	code += "\n\t" + tab_if_required + "addToJsonString(JS,\"\\\",\");"
	if fieldi[2] > 0 :
		code += "\n\t}"
		code += "\n\telse"
		code += "\n\t{"
		code += "\n\t\taddToJsonString(JS,\"null,\");"
		code += "\n\t}"
	code += "\n"
	return code

def forJson_forBoolean(fieldi) :
	pointer_variable = "(object->" + fieldi[1] + ")"
	code = ""
	code += "\n\t\tif( value != NULL )"
	code += "\n\t\t{"
	code += "\n\t\t\tif( value->type == TRUE_JSON )"
	code += "\n\t\t\t{"
	code += "\n\t\t\t\t" + pointer_variable + " = 1;"
	code += "\n\t\t\t}"
	code += "\n\t\t\telse if( value->type == FALSE_JSON )"
	code += "\n\t\t\t{"
	code += "\n\t\t\t\t" + pointer_variable + " = 0;"
	code += "\n\t\t\t}"
	code += "\n\t\t}"
	return code

def toJson_forBoolean(fieldi) :
	code  = ""
	code += "\n\taddToJsonString(JS,\"\\\"" + fieldi[1] + "\\\"\");"
	pointer_variable = "(object->" + fieldi[1] + ")"
	tab_if_required = ""
	if fieldi[2] > 0 :
		tab_if_required = "\t"
		code += "\n\tif( "
		for i in range(fieldi[2]) :
			code += "( ( " + pointer_variable + " ) != NULL )"
			if i != fieldi[2]-1 : 
				code += " && "
			i -= 1
			pointer_variable = "*" + pointer_variable
		code += " )"
		code += "\n\t{"
	code += "\n\t" + tab_if_required + "if( " + pointer_variable + " )"
	code += "\n\t" + tab_if_required + "{"
	code += "\n\t\t" + tab_if_required + "addToJsonString(JS,\":true,\");"
	code += "\n\t" + tab_if_required + "}"
	code += "\n\t" + tab_if_required + "else"
	code += "\n\t" + tab_if_required + "{"
	code += "\n\t\t" + tab_if_required + "addToJsonString(JS,\":false,\");"
	code += "\n\t" + tab_if_required + "}"
	if fieldi[2] > 0 :
		code += "\n\t}"
		code += "\n\telse"
		code += "\n\t{"
		code += "\n\t\taddToJsonString(JS,\":null,\");"
		code += "\n\t}"
	code += "\n"
	return code

def forJson_forObject(fieldi) :
	is_other = False
	datatype_name_string = "array_json"
	if fieldi[0] == DataType.OTHER :
		is_other = True
		datatype_name_string = fieldi[3]
	code = ""
	code  = ""
	code += "\n\t\tif( value != NULL && value->type == " + ( "OBJECT_JSON" if is_other else "ARRAY_JSON" ) + " )"
	code += "\n\t\t{"
	code += "\n\t\t\t"
	code += "\n\t\t}"
	return code


def toJson_forObject(fieldi) :
	datatype_name_string = "array_json"
	if fieldi[0] == DataType.OTHER :
		datatype_name_string = fieldi[3]
	code  = ""
	code += "\n\taddToJsonString(JS,\"\\\"" + fieldi[1] + "\\\":\");"
	pointer_variable = "(object->" + fieldi[1] + ")"
	tab_if_required = ""
	if fieldi[2] > 0 :
		tab_if_required = "\t"
		code += "\n\tif( "
		for i in range(fieldi[2]) :
			code += "( ( " + pointer_variable + " ) != NULL )"
			if i != fieldi[2]-1 : 
				code += " && "
			i -= 1
			pointer_variable = "*" + pointer_variable
		code += " )"
		code += "\n\t{"
	if fieldi[2] == 0 :
		pointer_address = "&(object->" + fieldi[1] + ")"
	else :
		pointer_address = pointer_variable[1:]
	code += "\n\t" + tab_if_required + "resultJsonObject = " + datatype_name_string + "_toJson( (" + pointer_address + ") );"
	code += "\n\t" + tab_if_required + "if( strcmp(resultJsonObject,\"{}\")==0 )"
	code += "\n\t" + tab_if_required + "{"
	code += "\n\t\t" + tab_if_required + "addToJsonString(JS,\"null,\");"
	code += "\n\t" + tab_if_required + "}"
	code += "\n\t" + tab_if_required + "else"
	code += "\n\t" + tab_if_required + "{"
	code += "\n\t\t" + tab_if_required + "addToJsonString(JS,resultJsonObject);"
	code += "\n\t\t" + tab_if_required + "addToJsonString(JS,\",\");"
	code += "\n\t" + tab_if_required + "}"
	code += "\n\t" + tab_if_required + "free(resultJsonObject);"
	if fieldi[2] > 0 :
		code += "\n\t}"
		code += "\n\telse"
		code += "\n\t{"
		code += "\n\t\taddToJsonString(JS,\"null,\");"
		code += "\n\t}"
	code += "\n"
	return code

def to_json_function_creator(json_object_name,fields) :
	function_string      = ""
	function_string     += "\nchar* " + json_object_name + "_toJson( " + json_object_name + "* object )"
	function_declaration = function_string + ";"
	function_string     += "\n{"
	function_string     += "\n"
	function_string     += "\n\tchar number[20];"
	function_string     += "\n\tchar* resultJsonObject;"
	function_string     += "\n\tJsonString* JS;"
	function_string     += "\n"
	function_string     += "\n\tif( object == NULL )"
	function_string     += "\n\t{"
	function_string     += "\n\t\tJS = getJsonString(\"{}\");"
	function_string     += "\n\t\tgoto exit;"
	function_string     += "\n\t}"

	if len(fields) == 0 :
		function_string += "\n\tJS = getJsonString(\"{}\");"
	else :
		function_string += "\n\tJS = getJsonString(\"{\");"

	function_string     += "\n"

	for fieldi in fields:
		if fieldi[0] == DataType.OTHER or fieldi[0] == DataType.ARRAY :
			function_string += toJson_forObject(fieldi)
		elif fieldi[0] == DataType.STRING :
			function_string += toJson_forString(fieldi)
		elif fieldi[0] == DataType.BOOLEAN :
			function_string += toJson_forBoolean(fieldi)
		else :
			function_string += toJson_forNumber(fieldi)

	if len(fields) > 0 :
		function_string += "\n\tJS->string[JS->size-2] = '}';"
		function_string += "\n"
	function_string     += "\n\texit:\t;"
	function_string     += "\n\tchar* result = JS->string;"
	function_string     += "\n\tfree(JS);"
	function_string     += "\n\treturn result;"
	function_string     += "\n}"
	return function_string

def from_json_function_creator(json_object_name,fields) :
	function_string      = ""
	function_string     += "\n" + json_object_name + "* " + json_object_name + "_fromJson( json_node* json )"
	function_declaration = function_string + ";"
	function_string     += "\n{"
	function_string     += "\n\tif( json == NULL || json->type == NULL_JSON )"
	function_string     += "\n\t{"
	function_string     += "\n\t\treturn NULL;"
	function_string     += "\n\t}"
	function_string     += "\n\t"
	function_string		+= "\n\t" + json_object_name + "* object = get_" + json_object_name + "();"
	function_string     += "\n\tjson_node* required_key = NULL;"
	function_string     += "\n\t"

	for fieldi in fields:
		function_string += "\n\trequired_key = find_key(json,\"" + fieldi[1] + "\");"
		function_string += "\n\tif( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )"
		function_string += "\n\t{"
		function_string += "\n\t\tjson_node* value = required_key->child;"
		if fieldi[0] == DataType.OTHER or fieldi[0] == DataType.ARRAY :
			function_string += forJson_forObject(fieldi)
		elif fieldi[0] == DataType.STRING :
			function_string += forJson_forString(fieldi)
		elif fieldi[0] == DataType.BOOLEAN :
			function_string += forJson_forBoolean(fieldi)
		else :
			function_string += forJson_forNumber(fieldi)
		function_string += "\n\t}"
		function_string += "\n\t"

	function_string		+= "\n\treturn object;"
	function_string     += "\n}"
	return function_string


