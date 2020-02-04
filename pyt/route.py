"""

command is used as below

python3 pyt/route.py <routing_file_0> <routing_file_1> <routing_file_2> <routing_file_3> ...
do not add con to the end of the routing file name,
routing files all should be present in the con folder only,
routing file must have a .con extension at the end,
routing file must contain be a json array

example. : 
[
	{
		"controller"				:	"first_controller",
		"methods"					:	["GET","POST"],
		"paths"						:	["/index/get_post"],
		"set_response_headers"		:	{
			"custom_api_specific_header"	:	"custom_header_value"
		}
	},
	{
		"methods"		:	["DELETE","POST"],
		"paths"			:	["/index/delete_post"],
		"controller"	:	"first_controller"
	},
	{
		"methods"		:	["GET"],
		"paths"			:	["/doc/go_google"],
		"redirect_to"	:	{
			"status"	:	303,
			"url"		:	"http://www.google.com"
		}
	},
	{
		"controller"	:	"wild_controller",
		"methods"		:	["*"],
		"paths"			:	["/wild/*"]
	}
]

"""


import json;
import strhsh;
import replace;


"""
#temporary hash to test
def getHashValue(s):
	return len(s)
"""

# read all commandline args
import sys
command_line_args = sys.argv.copy()[1:]

# below is the list of http methods supported by the serc framework
supported_methods = ["GET", "POST", "PUT", "DELETE",
			"HEAD", "PATCH", "OPTIONS",
			"TRACE", "CONNECT"]

# all the routing are stored in a global dictionary
# hence as each routing file is used conflicts are resolved by
# simply using the latter file as the higher priority
mydict = {}

# this is the array containting the names of all the controller functions in the system
controllers_list = []

# loop throught all the routing files and collect all paths
for routing_file in command_line_args:

	# a routing configuration file must end with .con
	if not routing_file.endswith(".con") :
		continue

	# read the file that contains route configurations
	routing_config_file = open(routing_file, "r")

	# since the file is in json use json.loads to use it as onject
	routes = json.loads(routing_config_file.read())

	routing_config_file.close()

	# use a dictionery to store the routes in format as required for making switch case
	# format is mydict[method][hashvalue_of_path][path_in_string] = controller
	# the below given loop builds up structure mydict for distributer.c and controllers_list for controller.h
	for route in routes :

		# if routing methods is a string, and it is "*", it means the context is all the methods
		if isinstance(route['methods'], str) :
			if route['methods'] == "*" :
				route['methods'] = supported_methods
			else :
				route['methods'] = [route['methods']]
		elif isinstance(route['methods'], list) :
			if "*" in route['methods'] :
				route['methods'] = supported_methods

		for method in route['methods']:
			if not (method in mydict):
				mydict[method] = {}
			for path in route['paths']:

				path_route_hash = None

				if "*" in path :
					if not ("wild_card_paths" in mydict[method]):
						mydict[method]["wild_card_paths"] = {}
					if not (path in mydict[method]["wild_card_paths"]):
						mydict[method]["wild_card_paths"][path] = {}
						path_route_hash = mydict[method]["wild_card_paths"][path]
				else :
					hashval = strhsh.getHashValue(path)
					if not (hashval in mydict[method]):
						mydict[method][hashval] = {}
					if not (path in mydict[method][hashval]):
						mydict[method][hashval][path] = {}
						path_route_hash = mydict[method][hashval][path]

				# this is the controller function that will be called if, 
				# the METHOD and PATH satisfy the condition, and they match in the request
				if 'controller' in route :
					controller = route['controller']
					path_route_hash['controller'] = controller
					# add the controller to the list of the controllers
					if not (controller in controllers_list) :
						controllers_list += [controller]

				# this is the redirection that will be used if, 
				# the METHOD and PATH satisfy the condition, and they match in the request
				if 'redirect_to' in route :
					path_route_hash['redirect_to'] = route['redirect_to']

				# the headers will the set for the response of every request,
				# if it hits this controller
				if 'set_response_headers' in route :
					path_route_hash['set_response_headers'] = route['set_response_headers']








"""
outer most routing part is switch by HttpMethodType
after that we route by the hash value of path
but note that any 2 strings can and may have same hash value
so for this we compare all the strings having the same hash value
"""
# create whole of case statement in case_string
case_string              			 = "\tswitch(METHOD)"
case_string             			+= "\n\t{"
for method in mydict:
	case_string         			+= "\n\t\tcase " + method + " :"
	case_string         			+= "\n\t\t{"
	case_string         			+= "\n\t\t\tswitch(PATH)"
	case_string         			+= "\n\t\t\t{"
	for hashval in mydict[method]:
		# wild card paths will be handled by the default case
		if hashval == "wild_card_paths" :
			continue
		case_string     			+= "\n\t\t\t\tcase " + str(hashval) + " :"
		case_string     			+= "\n\t\t\t\t{"
		for path in mydict[method][hashval]:
			case_string 			+= "\n\t\t\t\t\t// case for path = " + path + " and supports method = " + method
			case_string 			+= "\n\t\t\t\t\tif( 0 == strcmp(path_str, \"" + path + "\") )"
			case_string 			+= "\n\t\t\t\t\t{"
			if ('set_response_headers' in mydict[method][hashval][path]) and mydict[method][hashval][path]['set_response_headers'] is not None :
				case_string 		+= "\n\t\t\t\t\t\t// now here we add headers to the response, that we have to send"
				for header_key, header_value in mydict[method][hashval][path]['set_response_headers'].items() :
					case_string 	+= "\n\t\t\t\t\t\taddHeader(\"" + header_key + "\", \"" + header_value + "\", hrp->headers);"
			if ('controller' in mydict[method][hashval][path]) and mydict[method][hashval][path]['controller'] is not None :
				case_string 		+= "\n\t\t\t\t\t\trouting_resolved = 1;"
				case_string 		+= "\n\t\t\t\t\t\thrp->status = 200;"
				case_string 		+= "\n\t\t\t\t\t\terror = " + mydict[method][hashval][path]['controller'] + "(hrq, hrp);"
			if ('redirect_to' in mydict[method][hashval][path]) and mydict[method][hashval][path]['redirect_to'] is not None :
				case_string 		+= "\n\t\t\t\t\t\trouting_resolved = 1;"
				status = -1
				if ('with_status' in mydict[method][hashval][path]['redirect_to']) :
					status = mydict[method][hashval][path]['redirect_to']['with_status']
				case_string 		+= "\n\t\t\t\t\t\tredirectTo(" + str(status) + ", \"" + mydict[method][hashval][path]['redirect_to']['url'] + "\", hrp);"
			case_string 			+= "\n\t\t\t\t\t}"
		case_string     			+= "\n\t\t\t\t\tbreak;"
		case_string     			+= "\n\t\t\t\t}"
	if ("wild_card_paths" in mydict[method]) and len(mydict[method]["wild_card_paths"]):
		case_string					+= "\n\t\t\t\tdefault : "
		case_string					+= "\n\t\t\t\t{"
		case_string					+= "\n\t\t\t\t\tint wild_card_offset = 0;"
		for path in mydict[method]["wild_card_paths"]:
			case_string 			+= "\n\t\t\t\t\t// case for path = " + path + " and supports method = " + method
			path_parts = path.split("*")
			case_string 			+= "\n\t\t\t\t\tif( (" + str(len("".join(path_parts))) + " <= path_len)"
			itera = 0
			for path_part in path_parts :
				if path_part != "" :
					case_string			+= " and (0 == strncmp(path_str +, \"" + path_part + "\", " + str(len(path_part)) + "))"
					itera += 1
			case_string				+= " )"
			case_string 			+= "\n\t\t\t\t\t{"
			if ('set_response_headers' in mydict[method]["wild_card_paths"][path]) and mydict[method]["wild_card_paths"][path]['set_response_headers'] is not None :
				case_string 		+= "\n\t\t\t\t\t\t// now here we add headers to the response, that we have to send"
				for header_key, header_value in mydict[method]["wild_card_paths"][path]['set_response_headers'].items() :
					case_string 	+= "\n\t\t\t\t\t\taddHeader(\"" + header_key + "\", \"" + header_value + "\", hrp->headers);"
			if ('controller' in mydict[method]["wild_card_paths"][path]) and mydict[method]["wild_card_paths"][path]['controller'] is not None :
				case_string 		+= "\n\t\t\t\t\t\trouting_resolved = 1;"
				case_string 		+= "\n\t\t\t\t\t\thrp->status = 200;"
				case_string 		+= "\n\t\t\t\t\t\terror = " + mydict[method]["wild_card_paths"][path]['controller'] + "(hrq, hrp);"
			if ('redirect_to' in mydict[method]["wild_card_paths"][path]) and mydict[method]["wild_card_paths"][path]['redirect_to'] is not None :
				case_string 		+= "\n\t\t\t\t\t\trouting_resolved = 1;"
				status = -1
				if ('with_status' in mydict[method]["wild_card_paths"][path]['redirect_to']) :
					status = mydict[method]["wild_card_paths"][path]['redirect_to']['with_status']
				case_string 		+= "\n\t\t\t\t\t\tredirectTo(" + str(status) + ", \"" + mydict[method]["wild_card_paths"][path]['redirect_to']['url'] + "\", hrp);"
			case_string 			+= "\n\t\t\t\t\t}"
		case_string					+= "\n\t\t\t\t\thrp->status = 404;"
		case_string					+= "\n\t\t\t\t}"
	case_string         			+= "\n\t\t\t}"
	case_string         			+= "\n\t\t\tbreak;"
	case_string         			+= "\n\t\t}"
case_string							+= "\n\t\tdefault :"
case_string							+= "\n\t\t{"
case_string							+= "\n\t\t\thrp->status = 404;"
case_string							+= "\n\t\t}"
case_string             			+= "\n\t}\n"
		


declarations = ""
for function_name in controllers_list :
	declarations += "\n\tint " + function_name + "(HttpRequest* hrq,HttpResponse* hrp);"





replace.replaceLineWithCode("../pyt/distributer_source.temp","../src/distributer.c",{"//@switch_case\n":case_string})


replace.replaceLineWithCode("../pyt/controller_header.temp","../inc/controller.h",{"//@controller_definitions\n":declarations})


