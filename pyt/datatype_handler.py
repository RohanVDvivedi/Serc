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
	OTHER = 9
	ARRAY = 10

dataTypeStrings = {
	DataType.UNSIGNED_LONG_LONG_INT:['unsigned long long int','unsigned long long'],
	DataType.SIGNED_LONG_LONG_INT:['long long int','long long','signed long long int'],
	DataType.UNSIGNED_LONG_INT:['unsigned long int','unsigned long','unsigned long int'],
	DataType.SIGNED_LONG_INT:['long int','long','signed long int'],
	DataType.UNSIGNED_INT:['unsigned int'],
	DataType.SIGNED_INT:['int','signed int'],
	DataType.CHARACTER:['char','signed char'],
	DataType.UNSIGNED_CHARACTER:['unsigned char'],
	DataType.STRING:['char*','char *']
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
	DataType.CHARACTER:['char']

	DataType.ARRAY:['array_']
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
	DataType.STRING:'%s'
}

def forNumber(fieldi) :
	code  = ""
	code += "\n\tsprintf(number,\"" + dataTypeFormatSpecifierStrings[int(fieldi[0])] + ",\",object->" + fieldi[1] + ");"
	code += "\n\taddToJsonString(JS,\"\\\"" + fieldi[1] + "\\\":\");"
	code += "\n\taddToJsonString(JS,number);"
	code += "\n"
	return code

def forString(fieldi) :
	code  = ""
	code += "\n\taddToJsonString(JS,\"\\\"" + fieldi[1] + "\\\":\\\"\");"
	code += "\n\taddToJsonString(JS,object->" + fieldi[1] + ");"
	code += "\n\taddToJsonString(JS,\"\\\",\");"
	code += "\n"
	return code

def forObject(fieldi) :
	code  = ""
	code += "\n\taddToJsonString(JS,\"\\\"" + fieldi[1] + "\\\":\");"
	code += "\n\tchar* resultJsonObject = " + fieldi[2] + "_toJson(object->" + fieldi[1] + ");"
	code += "\n\taddToJsonString(JS,resultJsonObject);"
	code += "\n\tfree(resultJsonObject);"
	code += "\n"
	return code

def to_json_function_creator(json_object_name,fields):
	function_string      = ""
	function_string     += "\nchar* " + json_object_name + "_toJson( " + json_object_name + "* object )"
	function_declaration = function_string + ";"
	function_string     += "\n{"
	function_string     += "\n"
	function_string     += "\n\tchar number[20];"

	if len(fields) == 0 :
		function_string += "\n\tJsonString* JS = getJsonString(\"{}\");"
	else :
		function_string += "\n\tJsonString* JS = getJsonString(\"{\");"

	function_string     += "\n"

	for fieldi in fields:
		if fieldi[0] == DataType.OTHER :
			function_string += forObject(fieldi)
		elif fieldi[0] == DataType.STRING :
			function_string += forString(fieldi)
		else :
			function_string += forNumber(fieldi)

	if len(fields) > 0 :
		function_string += "\n\tJS->string[JS->size-2] = '}';"
	function_string     += "\n\tchar* result = JS->string;"
	function_string     += "\n\tfree(JS);"
	function_string     += "\n\treturn result;"
	function_string     += "\n}"
	return function_string