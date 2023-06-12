#!/usr/bin/env python3

"""

command is used as below

python3 pyt/route.py <routing_file_0> <routing_file_1> <routing_file_2> <routing_file_3> ...
routing file must have a .con extension at the end,
routing file must be a json array

example. : 
[
	{
		"controller"	:	"first_controller",
		"construct_per_request_param" : "new_per_request_param",
		"before"		:	["call_before_controller"],
		"after"			:	["call_after_controller"],
		"destroy_per_request_param" : "del_per_request_param",
		"methods"		:	["GET","POST"],
		"paths"			:	["/index/get_post"]
	},
	{
		"controller"	:	"first_controller",
		"before"		:	"call_before_controller",
		"after"			:	"call_after_controller",
		"methods"		:	["DELETE","POST"],
		"paths"			:	["/index/delete_post"]
	},
	{
		"controller"	:	"redirect_to_google",
		"methods"		:	"GET",
		"paths"			:	["/doc/go_google"]
	},
	{
		"controller"	:	"wildcard_controller",
		"before"		:	["call_before_controller"],
		"after"			:	["call_after_controller"],
		"methods"		:	"*",
		"paths"			:	["/wild/*/personal/*/hello/*", "/all/*"]
	},
	{
		"controller"	:	"cookie_setup_controller",
		"before"		:	["call_before_controller"],
		"after"			:	["call_after_controller"],
		"methods"		:	"*",
		"paths"			:	["/cookie_setup"]
	}
]

"""

# for parsing the routing.con file
import json
# for reading and writing files from disk
# and also accessing the command line variables passed
import sys
# to get path of the current file
import os

# gets the directory path where the script is stored
# the template file distributer_source.temp, must be stored in share folder right beside the directory of this script file
dir_path = os.path.dirname(os.path.realpath(__file__))

# same as getHashValue function in c in strhsh (it must be same as the implementation you find in c source code of serc)
# this has function will also be encountered in c (src/strhsh,h), which helps in routing request by hashing them
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

# the below function takes a template filename and code file name to edit
# it checks for each line in templateFile,
# if that line is found as key in the compareCodeHash, 
# them the value corresponding to the key in compareCodeHash, is pasted in the codeFile
# else it will copy paste the same line from templateFile to the codeFile
def replaceLineWithCode(templateFileName,codeFileName,compareCodeHash):
	global dir_path
	templatefile = open(dir_path + "/../share/" + templateFileName,"r");
	codefile = open(codeFileName,"w");

	for line in templatefile:
		test = line
		if test in compareCodeHash:
			codefile.write(compareCodeHash[line])
		else:
			codefile.write(line)

	templatefile.close();
	codefile.close();

# read all commandline args
command_line_args = sys.argv.copy()[1:]

# below is the list of http methods supported by the serc framework
supported_methods = ["GET", "POST", "PUT", "DELETE",
					"HEAD", "PATCH", "OPTIONS",
					"TRACE", "CONNECT"]

# all the routing are stored in a global dictionary
# hence as each routing file is used conflicts are resolved by
# simply using the latter file as the higher priority
mydict = {}

# this is the set containting the names of all the controller functions in the system
controller_like_function_declarations = set()

#declarations for contructiona and destroy per request param
construct_per_request_param_declaraions = set()
destroy_per_request_param_declarations = set()

# loop throught all the routing files and collect all paths
for routing_file in command_line_args:

	# a routing configuration file must end with .con
	if not routing_file.endswith(".con") :
		continue

	# read the file that contains route configurations
	routing_config_file = open(routing_file, "r")

	# since the file is in json use json.loads to use it as object
	routes = json.loads(routing_config_file.read())

	routing_config_file.close()

	# use a dictionery to store the routes in format as required for making switch case
	# format is mydict[method][hashvalue_of_path][path_in_string] = controller
	# the below given loop builds up structure mydict for distributer.c and controller_like_function_declarations for controller.h
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
					hashval = getHashValue(path)
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
					# add the controller to the controller_like_function_declarations set
					controller_like_function_declarations.add(controller)

				# controller like methods that get called before and after the controller
				if 'before' in route :
					if isinstance(route['before'], str) :
						path_route_hash['before'] = [route['before']]
						controller_like_function_declarations.add(route['before'])
					elif isinstance(route['before'], list) :
						path_route_hash['before'] = route['before']
						controller_like_function_declarations.union(set(path_route_hash['before']))

				if 'after' in route :
					if isinstance(route['after'], str) :
						path_route_hash['after'] = [route['after']]
						controller_like_function_declarations.add(route['after'])
					elif isinstance(route['after'], list) :
						path_route_hash['after'] = route['after']
						controller_like_function_declarations.union(set(path_route_hash['after']))

				if 'construct_per_request_param' in route :
					path_route_hash['construct_per_request_param'] = route['construct_per_request_param']
					construct_per_request_param_declaraions.add(route['construct_per_request_param'])

				if 'destroy_per_request_param' in route :
					path_route_hash['destroy_per_request_param'] = route['destroy_per_request_param']
					destroy_per_request_param_declaraions.add(route['destroy_per_request_param'])






# below for loop builds the switch case statements for routing the request to specified controller

"""
outer most routing part is switch by http_method
after that we route by the hash value of path
but note that any 2 strings can and may have same hash value
so for this we compare all the strings having the same hash value
"""
# create whole of case statement in case_string
case_string              			 = "\tswitch(hrq->method)"
case_string             			+= "\n\t{"
for method in mydict:
	case_string         			+= "\n\t\tcase " + method + " :"
	case_string         			+= "\n\t\t{"
	case_string         			+= "\n\t\t\tswitch(PATH_HASH)"
	case_string         			+= "\n\t\t\t{"
	for hashval in mydict[method]:
		# wild card paths will be handled by the default case
		if hashval == "wild_card_paths" :
			continue
		case_string					+= "\n\t\t\t\tcase " + str(hashval) + " :"
		case_string					+= "\n\t\t\t\t{"
		for path in mydict[method][hashval]:
			case_string 			+= "\n\t\t\t\t\t// case for path = " + path + " and supports method = " + method
			case_string 			+= "\n\t\t\t\t\tif( 0 == compare_dstring(&(hrq->path), &get_dstring_pointing_to_literal_cstring(\"" + path + "\")) )"
			case_string 			+= "\n\t\t\t\t\t{"
			case_string 			+= "\n\t\t\t\t\t\trouting_resolved = 1;"
			if("construct_per_request_param" in mydict[method][hashval][path]) and mydict[method][hashval][path]['construct_per_request_param'] is not None :
				case_string 		+= "\n\t\t\t\t\t\tvoid* per_request_param = " + mydict[method][hashval][path]['construct_per_request_param'] + "(server_param);"
			else :
				case_string 		+= "\n\t\t\t\t\t\tvoid* per_request_param = NULL;"
			if ('before' in mydict[method][hashval][path]) and mydict[method][hashval][path]['before'] is not None :
				for before in mydict[method][hashval][path]['before'] :
					case_string 	+= "\n\t\t\t\t\t\tclose_connection = close_connection || " + before + "(hrq, strm, per_request_param, sgp->server_param);"
			if ('controller' in mydict[method][hashval][path]) and mydict[method][hashval][path]['controller'] is not None :
				case_string 		+= "\n\t\t\t\t\t\tclose_connection = close_connection || " + mydict[method][hashval][path]['controller'] + "(hrq, strm, per_request_param, sgp->server_param);"
			if ('after' in mydict[method][hashval][path]) and mydict[method][hashval][path]['after'] is not None :
				for after in mydict[method][hashval][path]['after'] :
					case_string 	+= "\n\t\t\t\t\t\tclose_connection = close_connection || " + after + "(hrq, strm, per_request_param, sgp->server_param);"
			if("destroy_per_request_param" in mydict[method][hashval][path]) and mydict[method][hashval][path]['destroy_per_request_param'] is not None :
				case_string 		+= "\n\t\t\t\t\t\t" + mydict[method][hashval][path]['destroy_per_request_param'] + "(per_request_param , server_param);"
			case_string 			+= "\n\t\t\t\t\t}"
		case_string     			+= "\n\t\t\t\t\tbreak;"
		case_string     			+= "\n\t\t\t\t}"
	if ("wild_card_paths" in mydict[method]) and len(mydict[method]["wild_card_paths"]):
		case_string					+= "\n\t\t\t\tdefault : "
		case_string					+= "\n\t\t\t\t{"
		first_wild_card = True
		for path in mydict[method]["wild_card_paths"]:
			case_string 			+= "\n\t\t\t\t\t// case for path = " + path + " and supports method " + method
			if first_wild_card == True :
				case_string			+= "\n\t\t\t\t\t"
				first_wild_card = False
			else :
				case_string 		+= "\n\t\t\t\t\telse "
			case_string				+= "if( match_path_with_path_regex(&(hrq->path), &get_dstring_pointing_to_literal_cstring(\"" + path + "\") ) )"
			case_string 			+= "\n\t\t\t\t\t{"
			case_string 			+= "\n\t\t\t\t\t\trouting_resolved = 1;"
			if("construct_per_request_param" in mydict[method]["wild_card_paths"][path]) and mydict[method]["wild_card_paths"][path]['construct_per_request_param'] is not None :
				case_string 		+= "\n\t\t\t\t\t\tvoid* per_request_param = " + mydict[method]["wild_card_paths"][path]['construct_per_request_param'] + "(server_param);"
			else :
				case_string 		+= "\n\t\t\t\t\t\tvoid* per_request_param = NULL;"
			if ('before' in mydict[method]["wild_card_paths"][path]) and mydict[method]["wild_card_paths"][path]['before'] is not None :
				for before in mydict[method]["wild_card_paths"][path]['before'] :
					case_string 	+= "\n\t\t\t\t\t\tclose_connection = close_connection || " + before + "(hrq, strm, per_request_param, sgp->server_param);"
			if ('controller' in mydict[method]["wild_card_paths"][path]) and mydict[method]["wild_card_paths"][path]['controller'] is not None :
				case_string 		+= "\n\t\t\t\t\t\tclose_connection = close_connection || " + mydict[method]["wild_card_paths"][path]['controller'] + "(hrq, strm, per_request_param, sgp->server_param);"
			if ('after' in mydict[method]["wild_card_paths"][path]) and mydict[method]["wild_card_paths"][path]['after'] is not None :
				for after in mydict[method]["wild_card_paths"][path]['after'] :
					case_string 	+= "\n\t\t\t\t\t\tclose_connection = close_connection || " + after + "(hrq, strm, per_request_param, sgp->server_param);"
			if("destroy_per_request_param" in mydict[method]["wild_card_paths"][path]) and mydict[method]["wild_card_paths"][path]['destroy_per_request_param'] is not None :
				case_string 		+= "\n\t\t\t\t\t\t" + mydict[method]["wild_card_paths"][path]['destroy_per_request_param'] + "(per_request_param , server_param);"
			case_string 			+= "\n\t\t\t\t\t}"
		case_string					+= "\n\t\t\t\t\tbreak;"
		case_string					+= "\n\t\t\t\t}"
	case_string         			+= "\n\t\t\t}"
	case_string         			+= "\n\t\t\tbreak;"
	case_string         			+= "\n\t\t}"
case_string							+= "\n\t\tdefault :"
case_string							+= "\n\t\t{"
case_string							+= "\n\t\t\tbreak;"
case_string							+= "\n\t\t}"
case_string             			+= "\n\t}\n"
		

# below for loop builds forward declations for all the controllers that you will be using in your application
declarations = ""
for function_name in controller_like_function_declarations :
	declarations += "int " + function_name + "(const http_request_head* hrq, stream* strm, void* per_request_param, const void* server_param);\n"
for function_name in construct_per_request_param_declaraions :
	declarations += "void* " + function_name + "(const void* server_param);\n"
for function_name in construct_per_request_param_declaraions :
	declarations += "void " + function_name + "(void* per_request_param, const void* server_param);\n"





replaceLineWithCode(
						"distributer_source.temp",
						"distributer.c",
						{
							"//@controller_definitions\n":declarations,
							"//@switch_case\n":case_string
						}
					)
