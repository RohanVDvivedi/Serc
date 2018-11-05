import re;
import replace;
import enum;

'''
below function replaces all \n, \t, \r to just spaces individually
then strips out white spaces from front and back
then replaces all multiple spaces to single space
'''
def replaceWhiteSpaceWithSpace(string) :
	return re.sub(' +',' ',string.replace('\n',' ').replace('\t',' ').replace('\r',' ').strip())

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

dataTypeStrings = [
	['int','signed int'],
	['unsigned int'],
	['long int','long','signed long int'],
	['unsigned long int','unsigned long','unsigned long int'],
	['long long int','long long','signed long long int'],
	['unsigned long long int','unsigned long long'],
	['char','signed char'],
	['unsigned char'],
	['char*','char *']
]

jsonObjectName = "MyObject"


objectFileName = "../inc/DTOs_declarations/" + jsonObjectName + "json.h"
objectFileData = open(replace.dir_path + '/' + objectFileName,'r')

insideobject = 0

fieldString = ""

'''
the below piece of loop will read the c header to find all the text words between '{\n' and '};\n'
and put them in fieldString
'''
for line in objectFileData :
	for word in line.split(' ') :
		word = word.strip()
		if  word == "{" and insideobject == 0 :
			insideobject = 1
			continue
		elif word == "};" and insideobject == 1 :
			insideobject = 2
			break
		if insideobject == 1:
			fieldString += (word + " ");
	if insideobject == 2 :
		break


objectFileData.close()


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
	fieldi = fieldStr.strip().rsplit(" ",1)
	fields += [[replaceWhiteSpaceWithSpace(fieldi[0]), replaceWhiteSpaceWithSpace(fieldi[1])]]


'''
in the below loop we just transform DataType representations 
and identify them in specific enum as a normalizing step
this is required because 'int' and 'signed int' are same dataTypes
'''
for fieldi in fields :
	found = 0
	for id,listDataTypeString in enumerate(dataTypeStrings):
		if fieldi[0] in listDataTypeString :
			fieldi[0] = DataType(id)
			found = 1
	if found == 0:
		temp = fieldi[0]
		fieldi[0] = DataType.OTHER
		fieldi += [temp.replace("*","").replace(" ","")]


print(fields)


dataTypeFormatSpecifierStrings = [
	'%d',
	'%u',
	'%ld',
	'%lu',
	'%lld',
	'%llu',
	'%c',
	'%c',
	'%s'
]

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
	code += "\n\taddToJsonString(JS,\"\\\",\")"
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

function_string  = ""

function_string += "\nchar* " + jsonObjectName + "_toJson( " + jsonObjectName + "* object )"
function_declaration = function_string + ";"
function_string += "\n{"
function_string += "\n"
function_string += "\n\tchar number[20];"
function_string += "\n\tJsonString* JS = getJsonString(\"{\");"
function_string += "\n"

for fieldi in fields:
	if fieldi[0] == DataType.OTHER :
		function_string += forObject(fieldi)
	elif fieldi[0] == DataType.STRING :
		function_string += forString(fieldi)
	else :
		function_string += forNumber(fieldi)

function_string += "\n\tJS->string[JS->size-2] = '}';"
function_string += "\n\tchar* result = JS->string;"
function_string += "\n\tfree(JS);"
function_string += "\n\treturn result;"
function_string += "\n}"

print(function_string)




