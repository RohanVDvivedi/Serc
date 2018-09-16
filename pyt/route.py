import os;
import json;

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

# same as getHashValue function in c in strhsh
def getHashValue(s):
	ans = 0;i = 1;last = 0;curr = 0;diff = 0;lastoccur = [0] * 128;iter_i = 0
	while iter_i < len(s) :
		curr = ( ord(s[iter_i]) & 0x7f )
		if( i == 1 ):
			diff = 1
		else:
			diff = abs( last - curr ) + 1
		delta = curr * i * diff * (i - lastoccur[curr])
		ans = ans + delta;
		iter_i+=1
		last = curr
		lastoccur[curr] = i
		i+=1
		if i%5 == 0 :
			ans = ans + ((ans%13)*(diff+2)*(diff%3)*(int(diff/7))) + ((ans%29)*last) + ((ans%37)*curr) + (ans%11)
	return ans


"""
#temporary hash to test
def getHashValue(s):
	return len(s)
"""

# read the file that contains route configurations
routing_config_file = open("./con/routing.con","r")

# since the file is in json use json.loads to use it as onject
routes = json.loads(routing_config_file.read())

routing_config_file.close()

# use a dictionery to store the routes in format as required for making switch case
# format is mydict[method][hashvalue_of_path][path_in_string] = controller
# the below given loop builds up structure mydict for distributer.c and controllers_list for controller.h
mydict = {}
controllers_list = []
for route in routes :
	for method in route['methods']:
		if not (method in mydict):
			mydict[method] = {}
		for path in route['paths']:
			hashval = getHashValue(path)
			if not (hashval in mydict[method]):
				mydict[method][hashval] = {}
			mydict[method][hashval][path] = route['controller']
			if not (route['controller'] in controllers_list) :
				controllers_list += [route['controller']]








"""
outer most routing part is switch by HttpMethodType
after that we route by the hash value of path
but note that any 2 strings can and may have same hash value
so for this we compare all the strings having the same hash value
"""
# create whole of case statement in case_string
case_string              = "\tswitch(METHOD)"
case_string             += "\n\t{"
for method in mydict:
	case_string         += "\n\t\tcase " + method + " :"
	case_string         += "\n\t\t{"
	case_string         += "\n\t\t\tswitch(PATH)"
	case_string         += "\n\t\t\t{"
	for hashval in mydict[method]:
		case_string     += "\n\t\t\t\tcase " + str(hashval) + " :"
		case_string     += "\n\t\t\t\t{"
		for path in mydict[method][hashval]:
			case_string += "\n\t\t\t\t\t// case for path = " + path + " and supports method = " + method
			case_string += "\n\t\t\t\t\tif( 0 == strcmp( path_str , \"" + path + "\" ) )"
			case_string += "\n\t\t\t\t\t{"
			case_string += "\n\t\t\t\t\t\terror = " + mydict[method][hashval][path] + "(hrq,hrp);"
			case_string += "\n\t\t\t\t\t\trouting_resolved = 1;"
			case_string += "\n\t\t\t\t\t\thrp->Status = 200;"
			case_string += "\n\t\t\t\t\t}"
		case_string     += "\n\t\t\t\t\tbreak;"
		case_string     += "\n\t\t\t\t}"
	case_string         += "\n\t\t\t}"
	case_string         += "\n\t\t\tbreak;"
	case_string         += "\n\t\t}"
case_string				+= "\n\t\tdefault :"
case_string				+= "\n\t\t{"
case_string				+= "\n\t\t\thrp->Status = 404;"
case_string				+= "\n\t\t}"
case_string             += "\n\t}\n"
		


declarations = ""
for function_name in controllers_list :
	declarations += "\n\tint " + function_name + "(HttpRequest* hrq,HttpResponse* hrp);"





replaceLineWithCode("../pyt/distributer_source.temp","../src/bootstrapfiles/distributer.c",{"//@switch_case\n":case_string})


replaceLineWithCode("../pyt/controller_header.temp","../inc/bootstrapfiles/controller.h",{"//@controller_definitions\n":declarations})

